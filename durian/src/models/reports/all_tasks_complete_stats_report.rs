//! This file contains the implementation of AllTasksCompleteStatsReport which impl TasksSchedStatsReport.
//! Refer to this implementation's conventions when implementing other types of reports.

use std::fs::File;
use std::io::{BufWriter, Write};

use serde::{Deserialize, Serialize};

use crate::app::App;
use crate::config::Config;
use crate::core;
use crate::sched_math::{duration_ns_to_fmt_duration, nice_to_prio, prio_to_nice};

use super::TasksSchedStatsReport;
use crate::models::task_statistics::TaskStatistics;
use crate::models::tasks_states_counts::AllTasksStatesCounts;

/// The actual report struct that will be serialized and deserialized
/// from bincode. It will only take a snapshot of the Redis state.
/// It should contain as little computation as possible. Necessary computation
/// should happen when generating the intermediate form (PreprocessedReport).
#[derive(Debug, Serialize, Deserialize)]
pub struct AllTasksCompleteStatsReport {
    pub tasks_stats: Vec<TaskStatistics>,
}

/// The intermediate form of AllTasksCompleteStatsReport.
/// It derives data and information from the original report based on
/// a certain app configuration.
struct PreprocessedReport {
    pub num_tasks: usize,
    pub tasks_states_counts: AllTasksStatesCounts,

    pub avg_io_time_ns: f32,
    pub avg_cpu_time_ns: f32,
    pub tasks_stats: Vec<TaskStatistics>,
    pub tasks_normalized_cpu_fair_share_ns: Vec<f32>,
    pub tasks_ideal_normalized_cpu_fair_share_ns: Vec<f32>,

    // The priority values that correlates best to the actual fair share used
    pub tasks_actual_fair_share_nice: Vec<i16>,

    // The config values used for analysis
    pub config: Config,
}

#[typetag::serde(name = "all_tasks_complete_stats_report")]
impl TasksSchedStatsReport for AllTasksCompleteStatsReport {
    fn report(&self, filename: &str, app: &App) -> std::io::Result<()> {
        let file = File::create(filename)?;
        let mut writer = BufWriter::new(file);

        let preprocessed_report = self.preprocess_report(app);

        preprocessed_report.report_aggregate_sched_stats(&mut writer)?;

        writer.write_fmt(format_args!("\n"))?;

        preprocessed_report.report_sched_stats(&mut writer)?;

        writer.write_fmt(format_args!("\n"))?;

        preprocessed_report.report_sched_stats_analysis(&mut writer)?;

        writer.write_fmt(format_args!("\n"))?;

        preprocessed_report.report_configurations_used(&mut writer)?;

        writer.flush()?;

        Ok(())
    }
}

impl AllTasksCompleteStatsReport {
    fn preprocess_report(&self, app: &App) -> PreprocessedReport {
        let tasks_stats = &self.tasks_stats;
        let filtered_tasks_stats = core::filter_tasks(tasks_stats.to_vec(), app);
        let avg_io_time_ns = core::get_tasks_average_io_time(&filtered_tasks_stats);
        let avg_cpu_time_ns = core::get_tasks_average_cpu_time(&filtered_tasks_stats);
        let tasks_normalized_cpu_fair_share_ns = core::get_tasks_normalized_cpu_fair_share_ns(
            &filtered_tasks_stats,
            app.sched_latency_ns,
        );
        let tasks_ideal_normalized_cpu_fair_share_ns =
            core::get_tasks_ideal_normalized_cpu_fair_share_ns(
                &filtered_tasks_stats,
                app.sched_latency_ns,
            );

        let tasks_states_counts: AllTasksStatesCounts =
            core::get_all_tasks_states_count(&filtered_tasks_stats);
        let tasks_actual_fair_share_nice = core::calculate_tasks_actual_fair_share_prio(
            &tasks_normalized_cpu_fair_share_ns,
            &tasks_ideal_normalized_cpu_fair_share_ns,
        );

        let config = Config::read_config_from_app(app);

        PreprocessedReport {
            num_tasks: filtered_tasks_stats.len(),
            tasks_states_counts,
            tasks_actual_fair_share_nice,
            avg_io_time_ns,
            avg_cpu_time_ns,
            tasks_stats: filtered_tasks_stats,
            tasks_normalized_cpu_fair_share_ns,
            tasks_ideal_normalized_cpu_fair_share_ns,
            config,
        }
    }
}

impl PreprocessedReport {
    fn report_aggregate_sched_stats(&self, writer: &mut BufWriter<File>) -> std::io::Result<()> {
        writer.write_fmt(format_args!(
            "Tasks:{}{} total,{}{} on cpu,{}{} on run queue,{}{} waiting,{}{} stopped\n",
            " ".repeat(5),
            self.num_tasks,
            " ".repeat(5),
            self.tasks_states_counts.num_tasks_running_cpu,
            " ".repeat(5),
            self.tasks_states_counts.num_tasks_running_rq,
            " ".repeat(5),
            self.tasks_states_counts.num_tasks_waiting,
            " ".repeat(5),
            self.tasks_states_counts.num_tasks_stopped,
        ))?;

        writer.write_fmt(format_args!(
            "Average I/O: {} ns,{} average CPU: {} ns\n",
            self.avg_io_time_ns,
            " ".repeat(5),
            self.avg_cpu_time_ns
        ))?;
        Ok(())
    }

    fn report_sched_stats(&self, writer: &mut BufWriter<File>) -> std::io::Result<()> {
        writer.write_fmt(format_args!(
            "{} STATISTICS {}\n",
            "=".repeat(45),
            "=".repeat(45)
        ))?;

        writer.write_fmt(format_args!(
            "{}PID{}PRIO{}NICE{}TIME+{}COMMAND{}TOTAL_CPU(NS){}TOTAL_WAIT(NS){}NR_SWITCH\n",
            " ".repeat(4),
            " ".repeat(1),
            " ".repeat(2),
            " ".repeat(8),
            " ".repeat(11),
            " ".repeat(5),
            " ".repeat(4),
            " ".repeat(3),
        ))?;

        let n = self.tasks_stats.len();

        for i in 0..n {
            let t = &self.tasks_stats[i];

            writer.write_fmt(format_args!(
                "{:>7} {:>4} {:>5} {:>8} {:>18} {:>17} {:>17} {:>11}\n",
                t.pid,
                t.prio,
                prio_to_nice(t.prio),
                duration_ns_to_fmt_duration(t.last_ktime_ns - t.sched_stats_start_time_ns),
                t.comm,
                t.total_cpu_time_ns,
                t.total_wait_time_ns,
                t.nr_switches,
            ))?;
        }

        Ok(())
    }

    fn report_sched_stats_analysis(&self, writer: &mut BufWriter<File>) -> std::io::Result<()> {
        writer.write_fmt(format_args!(
            "{} ANALYSIS {}\n",
            "=".repeat(46),
            "=".repeat(46)
        ))?;
        writer.write_fmt(format_args!("\n"))?;

        writer.write_fmt(format_args!(
            "{}PID{}FAIR_NS{}IDEAL_FAIR_NS{}PRIO{}NICE{}FAIR_SHARE_PRIO{}FAIR_SHARE_NICE\n",
            " ".repeat(4),
            " ".repeat(11),
            " ".repeat(5),
            " ".repeat(4),
            " ".repeat(4),
            " ".repeat(4),
            " ".repeat(4),
        ))?;

        let n = self.tasks_stats.len();

        for i in 0..n {
            let t = &self.tasks_stats[i];
            let actual_share = &self.tasks_normalized_cpu_fair_share_ns[i];
            let ideal_share = &self.tasks_ideal_normalized_cpu_fair_share_ns[i];
            let actual_fair_share_nice = &self.tasks_actual_fair_share_nice[i];

            writer.write_fmt(format_args!(
                "{:>7} {:>17} {:>17} {:>7} {:>7} {:>18} {:>18}\n",
                t.pid,
                format!("{:.2}", actual_share),
                format!("{:.2}", ideal_share),
                t.prio,
                prio_to_nice(t.prio),
                nice_to_prio(*actual_fair_share_nice),
                format!("{:+}", actual_fair_share_nice),
            ))?;
        }

        writer.write_fmt(format_args!("\n"))?;
        writer.write_fmt(format_args!(
            "### NOTE : FAIR SHARES ARE NORMALIZED TO SCHED_LATENCY FOR ANALYSIS\n"
        ))?;
        writer.write_fmt(format_args!("\n"))?;

        Ok(())
    }

    fn report_configurations_used(&self, writer: &mut BufWriter<File>) -> std::io::Result<()> {
        writer.write_fmt(format_args!(
            "{} ANALYSIS CONFIGURATION {}\n",
            "=".repeat(39),
            "=".repeat(39)
        ))?;
        writer.write_fmt(format_args!("\n"))?;

        writer.write_fmt(format_args!("{}", self.config))?;

        Ok(())
    }
}
