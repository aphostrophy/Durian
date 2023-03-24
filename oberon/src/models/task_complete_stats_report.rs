use serde::{Deserialize, Serialize};

use super::task_statistics::TaskStatistics;
use super::tasks_sched_stats_report::TasksSchedStatsReport;

#[derive(Debug, Serialize, Deserialize)]
pub struct TaskCompleteStatsReport {
    pub task_stats: TaskStatistics,
}

#[typetag::serde(name = "task_complete_stats_report")]
impl TasksSchedStatsReport for TaskCompleteStatsReport {}
