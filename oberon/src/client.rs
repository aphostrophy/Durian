use std::fs::File;
use std::io::BufWriter;

use crate::app::{App, Command, ShowCommand};
use crate::core;
use crate::errors::OberonResult;
use crate::models::all_tasks_complete_stats_report::AllTasksCompleteStatsReport;
use crate::models::task_complete_stats_report::TaskCompleteStatsReport;
use crate::models::task_statistics::TaskStatistics;
use crate::models::tasks_sched_stats_report::TasksSchedStatsReport;
use crate::repository;

pub struct Client {
    repository: redis::Connection,
}

impl Client {
    pub fn new() -> Self {
        let redis_client = redis::Client::open("redis://127.0.0.1/");
        let con = match redis_client {
            Ok(redis_client) => redis_client.get_connection(),
            Err(err) => panic!("Failed to connect to Redis: {}", err),
        };

        match con {
            Ok(con) => Client { repository: con },
            Err(err) => panic!("Failed on opening Redis connection: {}", err),
        }
    }

    pub fn perform_action(&mut self, app: &App, action: &Command) -> OberonResult<()> {
        match action {
            Command::SHOW { show_command } => self.perform_show(app, show_command),
        }
    }

    fn perform_show(&mut self, app: &App, show_command: &Option<ShowCommand>) -> OberonResult<()> {
        let report = self.gen_show_sched_stats_report(app, show_command)?;

        let mut file = File::create("report.bin").unwrap();
        let writer = BufWriter::new(&mut file);
        bincode::serialize_into(writer, &report).unwrap();

        Ok(())
    }

    fn gen_show_sched_stats_report(
        &mut self,
        app: &App,
        show_command: &Option<ShowCommand>,
    ) -> OberonResult<Box<dyn TasksSchedStatsReport>> {
        match show_command {
            Some(ref command) => match command {
                ShowCommand::All(options) => match options.pid {
                    Some(ref pid) => {
                        let task_stats =
                            repository::gen_task_complete_statistics(&mut self.repository, pid)?;
                        Ok(Box::new(TaskCompleteStatsReport { task_stats }))
                    }
                    None => gen_all_tasks_complete_stats_report(&mut self.repository, app),
                },
            },
            None => gen_all_tasks_complete_stats_report(&mut self.repository, app),
        }
    }
}

/// Filters the tasks statistics based on app config.
///
/// Will do filtering based on:
///
/// - nr_switches to avoid new task bias (e.g. task with exactly 1 context switch might seem
/// like dominating the entire CPU).
///
/// - priority to only select tasks that is scheduled by SCHED_NORMAL scheduling policy.
///
/// # Arguments
///
/// `tasks_stats` - unfiltered tasks statistics
/// `app` - oberon app containing config fields
///
/// # Examples
/// . . .
fn filter_tasks(tasks_stats: Vec<TaskStatistics>, app: &App) -> Vec<TaskStatistics> {
    tasks_stats
        .into_iter()
        .filter(|t| t.nr_switches >= app.min_nr_switches && t.prio >= 100 && t.prio <= 139)
        .collect()
}

fn gen_all_tasks_complete_stats_report(
    repository: &mut redis::Connection,
    app: &App,
) -> OberonResult<Box<dyn TasksSchedStatsReport>> {
    let tasks_stats = repository::gen_all_tasks_complete_statistics(repository)?;
    let filtered_tasks_stats = filter_tasks(tasks_stats, app);
    let avg_io_time_ns = core::get_tasks_average_io_time(&filtered_tasks_stats);
    let avg_cpu_time_ns = core::get_tasks_average_cpu_time(&filtered_tasks_stats);
    let tasks_normalized_cpu_fair_share_ns =
        core::get_tasks_normalized_cpu_fair_share_ns(&filtered_tasks_stats);
    let tasks_ideal_normalized_cpu_fair_share_ns =
        core::get_tasks_ideal_normalized_cpu_fair_share_ns(&filtered_tasks_stats);

    Ok(Box::new(AllTasksCompleteStatsReport {
        num_tasks: filtered_tasks_stats.len(),
        avg_io_time_ns,
        avg_cpu_time_ns,
        tasks_stats: filtered_tasks_stats,
        tasks_normalized_cpu_fair_share_ns,
        tasks_ideal_normalized_cpu_fair_share_ns,
    }))
}
