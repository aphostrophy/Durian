pub mod interface;
pub use interface::tasks_sched_stats_report::TasksSchedStatsReport;

pub mod all_tasks_complete_stats_report;
pub mod task_complete_stats_report;

use all_tasks_complete_stats_report::AllTasksCompleteStatsReport;
use task_complete_stats_report::TaskCompleteStatsReport;

use crate::app::App;
use crate::errors::DurianResult;
use crate::repository;

/// Returns AllTasksCompleteStatsReport as TasksSchedStatsReport interface
///
/// # Arguments
///
/// `repository`: redis connection
/// `app`: durian app
pub fn gen_all_tasks_complete_stats_report(
    repository: &mut redis::Connection,
    _app: &App,
) -> DurianResult<Box<dyn TasksSchedStatsReport>> {
    let tasks_stats = repository::gen_all_tasks_complete_statistics(repository)?;
    Ok(Box::new(AllTasksCompleteStatsReport { tasks_stats }))
}

/// Returns AllTasksCompleteStatsReport as TasksSchedStatsReport interface
///
/// # Arguments
///
/// `repository`: redis connection
/// `app`: durian app
/// `pid`: pid of the analyzed task
pub fn gen_task_complete_stats_report(
    repository: &mut redis::Connection,
    _app: &App,
    pid: &i32,
) -> DurianResult<Box<dyn TasksSchedStatsReport>> {
    let task_stats = repository::gen_task_complete_statistics(repository, pid)?;
    Ok(Box::new(TaskCompleteStatsReport { task_stats }))
}
