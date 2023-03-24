#[cfg(test)]
mod tests;

use crate::models::task_statistics::TaskStatistics;

/// NOT to be confused with sched core in the linux kernel source code.
///
/// This module provides an abstraction for all non trivial task statistics
/// calculation and derivation.

/// Returns the sum of all task's I/O time divided by num tasks.
///
/// # Arguments
///
/// `tasks_stats`: tasks scheduling statistics.
pub fn get_tasks_average_io_time(tasks_stats: &Vec<TaskStatistics>) -> f32 {
    let sum = tasks_stats
        .iter()
        .fold(0i128, |acc, t| acc + t.total_wait_time_ns as i128);

    if tasks_stats.len() == 0 {
        return 0.0;
    }

    let avg = sum as f32 / tasks_stats.len() as f32;
    avg
}

/// Returns the sum of all task's CPU time divided by num tasks.
///
/// # Arguments
///
/// `tasks_stats`: tasks scheduling statistics.
pub fn get_tasks_average_cpu_time(tasks_stats: &Vec<TaskStatistics>) -> f32 {
    let sum = tasks_stats
        .iter()
        .fold(0i128, |acc, t| acc + t.total_cpu_time_ns as i128);

    if tasks_stats.len() == 0 {
        return 0.0;
    }

    let avg = sum as f32 / tasks_stats.len() as f32;
    avg
}

/// Returns the normalized CPU fair share for every tasks.
///
/// Normalized CPU fair share is calculated based on exiting task statistics. This function
/// uses the 'calculate_cpu_fair_share_per_period_ns' method of TaskStatistics, see the
/// documentation in oberon/src/models/task_statistics.rs
///
/// # Arguments
///
/// `tasks_stats`: tasks scheduling statistics.
pub fn get_tasks_normalized_cpu_fair_share_ns(tasks_stats: &Vec<TaskStatistics>) -> Vec<f32> {
    let period = 20 * 1000; // default 20ms (20 * 10^3 ns)
    tasks_stats
        .iter()
        .map(|t| t.calculate_cpu_fair_share_per_period_ns(period))
        .collect()
}
