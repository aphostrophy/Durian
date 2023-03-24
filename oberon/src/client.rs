use std::fs::File;
use std::io::BufWriter;

use crate::app::{App, Command, ShowCommand};
use crate::errors::OberonResult;
use crate::models::all_tasks_complete_stats_report::AllTasksCompleteStatsReport;
use crate::models::task_complete_stats_report::TaskCompleteStatsReport;
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
        _app: &App,
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
                    None => {
                        let tasks_stats =
                            repository::gen_all_tasks_complete_statistics(&mut self.repository)?;
                        let avg_io_time_ns = repository::get_tasks_average_io_time(&tasks_stats);
                        let avg_cpu_time_ns = repository::get_tasks_average_cpu_time(&tasks_stats);
                        Ok(Box::new(AllTasksCompleteStatsReport {
                            num_tasks: tasks_stats.len(),
                            avg_io_time_ns,
                            avg_cpu_time_ns,
                            tasks_stats,
                        }))
                    }
                },
            },
            None => {
                let tasks_stats =
                    repository::gen_all_tasks_complete_statistics(&mut self.repository)?;
                let avg_io_time_ns = repository::get_tasks_average_io_time(&tasks_stats);
                let avg_cpu_time_ns = repository::get_tasks_average_cpu_time(&tasks_stats);
                Ok(Box::new(AllTasksCompleteStatsReport {
                    num_tasks: tasks_stats.len(),
                    avg_io_time_ns,
                    avg_cpu_time_ns,
                    tasks_stats,
                }))
            }
        }
    }
}
