pub mod interface;
pub use interface::tasks_sched_stats_report::TasksSchedStatsReport;

pub mod all_tasks_complete_stats_report;
pub mod task_complete_stats_report;

use all_tasks_complete_stats_report::AllTasksCompleteStatsReport;
use task_complete_stats_report::TaskCompleteStatsReport;

use crate::app::App;
use crate::config::Config;
use crate::core;
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
    app: &App,
) -> DurianResult<Box<dyn TasksSchedStatsReport>> {
    let tasks_stats = repository::gen_all_tasks_complete_statistics(repository)?;
    let filtered_tasks_stats = core::filter_tasks(tasks_stats, app);
    let avg_io_time_ns = core::get_tasks_average_io_time(&filtered_tasks_stats);
    let avg_cpu_time_ns = core::get_tasks_average_cpu_time(&filtered_tasks_stats);
    let tasks_normalized_cpu_fair_share_ns =
        core::get_tasks_normalized_cpu_fair_share_ns(&filtered_tasks_stats, app.sched_latency_ns);
    let tasks_ideal_normalized_cpu_fair_share_ns =
        core::get_tasks_ideal_normalized_cpu_fair_share_ns(
            &filtered_tasks_stats,
            app.sched_latency_ns,
        );

    let tasks_states_counts: super::tasks_states_counts::AllTasksStatesCounts =
        core::get_all_tasks_states_count(&filtered_tasks_stats);
    let tasks_actual_fair_share_nice = core::calculate_tasks_actual_fair_share_prio(
        &tasks_normalized_cpu_fair_share_ns,
        &tasks_ideal_normalized_cpu_fair_share_ns,
    );

    let config = Config::read_config_from_app(app);

    Ok(Box::new(AllTasksCompleteStatsReport {
        num_tasks: filtered_tasks_stats.len(),
        tasks_states_counts,
        tasks_actual_fair_share_nice,
        avg_io_time_ns,
        avg_cpu_time_ns,
        tasks_stats: filtered_tasks_stats,
        tasks_normalized_cpu_fair_share_ns,
        tasks_ideal_normalized_cpu_fair_share_ns,
        config,
    }))
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
