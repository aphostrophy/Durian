use std::fs::File;
use std::io::{BufReader, BufWriter};

use crate::app::{App, Command, RecordCommand, ReportCommand};

use crate::errors::DurianResult;
use crate::models::reports::TasksSchedStatsReport;
use crate::models::reports::{gen_all_tasks_complete_stats_report, gen_task_complete_stats_report};

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

    pub fn perform_action(&mut self, app: &App, action: &Command) -> DurianResult<()> {
        match action {
            Command::RECORD { record_command } => self.perform_record(app, record_command),
            Command::REPORT { report_command } => self.perform_report(app, report_command),
        }
    }

    fn perform_record(
        &mut self,
        app: &App,
        record_command: &Option<RecordCommand>,
    ) -> DurianResult<()> {
        let report = self.gen_show_sched_stats_report(app, record_command)?;

        let mut file = File::create("report.bin")?;
        let writer = BufWriter::new(&mut file);
        bincode::serialize_into(writer, &report).unwrap();

        Ok(())
    }

    fn perform_report(
        &mut self,
        _app: &App,
        report_command: &Option<ReportCommand>,
    ) -> DurianResult<()> {
        let mut report_path = "report.bin".to_string();
        let mut output_path = "durian.data".to_string();
        if let Some(ref command) = report_command {
            match command {
                ReportCommand::All(options) => {
                    if let Some(path) = &options.path {
                        report_path = path.clone()
                    }
                    output_path = options.output_path.clone();
                }
            }
        }

        let tasks_sched_stats_report: Box<dyn TasksSchedStatsReport> =
            self.gen_deserialized_report(report_path)?;

        tasks_sched_stats_report.report(&output_path)?;

        Ok(())
    }

    fn gen_show_sched_stats_report(
        &mut self,
        app: &App,
        record_command: &Option<RecordCommand>,
    ) -> DurianResult<Box<dyn TasksSchedStatsReport>> {
        match record_command {
            Some(ref command) => match command {
                RecordCommand::All(options) => match options.pid {
                    Some(ref pid) => gen_task_complete_stats_report(&mut self.repository, app, pid),
                    None => gen_all_tasks_complete_stats_report(&mut self.repository, app),
                },
            },
            None => gen_all_tasks_complete_stats_report(&mut self.repository, app),
        }
    }

    fn gen_deserialized_report(
        &mut self,
        report_path: String,
    ) -> DurianResult<Box<dyn TasksSchedStatsReport>> {
        let file = File::open(report_path)?;
        let reader = BufReader::new(file);
        let tasks_sched_stats_report: Box<dyn TasksSchedStatsReport> =
            bincode::deserialize_from(reader).unwrap();

        Ok(tasks_sched_stats_report)
    }
}
