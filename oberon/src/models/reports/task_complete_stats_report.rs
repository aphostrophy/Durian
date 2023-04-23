use std::fs::File;
use std::io::{BufWriter, Write};

use serde::{Deserialize, Serialize};

use super::tasks_sched_stats_report::TasksSchedStatsReport;
use crate::models::task_statistics::TaskStatistics;

#[derive(Debug, Serialize, Deserialize)]
pub struct TaskCompleteStatsReport {
    pub task_stats: TaskStatistics,
}

#[typetag::serde(name = "task_complete_stats_report")]
impl TasksSchedStatsReport for TaskCompleteStatsReport {
    fn report(&self, filename: &str) -> std::io::Result<()> {
        let file = File::create(filename)?;
        let mut writer = BufWriter::new(file);

        // TODO: Implement

        writer.flush()?;

        Ok(())
    }
}
