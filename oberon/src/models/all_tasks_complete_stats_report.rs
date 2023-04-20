use std::fs::File;
use std::io::{BufWriter, Write};

use serde::{Deserialize, Serialize};

use crate::sched_math::{duration_ns_to_fmt_duration, prio_to_nice};

use super::task_statistics::TaskStatistics;
use super::tasks_sched_stats_report::TasksSchedStatsReport;
use super::tasks_states_counts::AllTasksStatesCounts;

#[derive(Debug, Serialize, Deserialize)]
pub struct AllTasksCompleteStatsReport {
    pub num_tasks: usize,
    pub tasks_states_counts: AllTasksStatesCounts,
    pub avg_io_time_ns: f32,
    pub avg_cpu_time_ns: f32,
    pub tasks_stats: Vec<TaskStatistics>,
    pub tasks_normalized_cpu_fair_share_ns: Vec<f32>,
    pub tasks_ideal_normalized_cpu_fair_share_ns: Vec<f32>,
}

#[typetag::serde(name = "all_tasks_complete_stats_report")]
impl TasksSchedStatsReport for AllTasksCompleteStatsReport {
    fn report(&self, filename: &str) -> std::io::Result<()> {
        let file = File::create(filename)?;
        let mut writer = BufWriter::new(file);

        self.report_aggregate_sched_stats(&mut writer)?;

        writer.write_fmt(format_args!("\n"))?;

        self.report_sched_stats(&mut writer)?;

        writer.write_fmt(format_args!("\n"))?;

        self.report_sched_stats_analysis(&mut writer)?;

        writer.flush()?;

        Ok(())
    }
}

impl AllTasksCompleteStatsReport {
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
        Ok(())
    }

    fn report_sched_stats(&self, writer: &mut BufWriter<File>) -> std::io::Result<()> {
        writer.write_fmt(format_args!(
            "{} STATISTICS {}\n",
            "=".repeat(5),
            "=".repeat(5)
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
            "=".repeat(5),
            "=".repeat(5)
        ))?;
        writer.write_fmt(format_args!("\n"))?;

        writer.write_fmt(format_args!(
            "{}PID{}FAIR_NS{}IDEAL_FAIR_NS{}PRIO{}NICE{}RECO_PRIO{}RECO_NICE\n",
            " ".repeat(4),
            " ".repeat(4),
            " ".repeat(4),
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

            writer.write_fmt(format_args!(
                "{:>7} {:>10} {:>16} {:>7} {:>7} {:>12} {:>12}\n",
                t.pid,
                format!("{:.2}", actual_share),
                ideal_share,
                t.prio,
                prio_to_nice(t.prio),
                "-",
                "-"
            ))?;
        }

        writer.write_fmt(format_args!("\n"))?;
        writer.write_fmt(format_args!(
            "### NOTE : FAIR SHARES ARE NORMALIZED TO SCHED_LATENCY FOR ANALYSIS\n"
        ))?;
        writer.write_fmt(format_args!("\n"))?;

        Ok(())
    }
}
