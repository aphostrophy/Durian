use serde::{Deserialize, Serialize};

use super::task_statistics::TaskStatistics;
use super::tasks_sched_stats_report::TasksSchedStatsReport;

#[derive(Debug, Serialize, Deserialize)]
pub struct AllTasksCompleteStatsReport {
    pub num_tasks: usize,
    pub avg_io_time_ns: f32,
    pub avg_cpu_time_ns: f32,
    pub tasks_stats: Vec<TaskStatistics>,
    pub tasks_normalized_cpu_fair_share_ns: Vec<f32>,
}

#[typetag::serde(name = "all_tasks_complete_stats_report")]
impl TasksSchedStatsReport for AllTasksCompleteStatsReport {}
