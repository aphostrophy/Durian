#[cfg(test)]
mod tests;

use crate::app::App;
use crate::sched_math::scale_prio_to_weight;
use crate::{
    models::task_statistics::TaskStatistics, models::tasks_states_counts::AllTasksStatesCounts,
};

use crate::oberon_def::{TASK_RUNNING_CPU, TASK_RUNNING_RQ, TASK_WAITING, __TASK_STOPPED};

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

/// Returns the normalized CPU fair share for every tasks for every period.
///
/// Normalized CPU fair share is calculated based on exiting task statistics. This function
/// uses the 'calculate_cpu_fair_share_per_period_ns' method of TaskStatistics, see the
/// documentation in oberon/src/models/task_statistics.rs
///
/// # Arguments
///
/// `tasks_stats`: tasks scheduling statistics.
pub fn get_tasks_normalized_cpu_fair_share_ns(
    tasks_stats: &Vec<TaskStatistics>,
    period: u64,
) -> Vec<f32> {
    let normalized_cpu_time_sum = get_tasks_normalized_cpu_time_sum(&tasks_stats, period);

    tasks_stats
        .iter()
        .map(|t| {
            t.calculate_cpu_fair_share_per_period_ns(period) / normalized_cpu_time_sum
                * period as f32
        })
        .collect()
}

/// Returns the ideal CPU fair share for every task for every period.
///
/// # Arguments
///
/// `tasks_stats`: tasks scheduling statistics
/// `period`: an imaginary time period that is assumed to be fairly shared among
///  the tasks in nanoseconds
///
/// Ideal here is defined as when a task fully use all of their fair share in the CPU.
pub fn get_tasks_ideal_normalized_cpu_fair_share_ns(
    tasks_stats: &Vec<TaskStatistics>,
    period: u64,
) -> Vec<f32> {
    let weight_sum = get_tasks_weight_sum(&tasks_stats);

    tasks_stats
        .iter()
        .map(|t| scale_prio_to_weight(t.prio) as f32 / weight_sum as f32 * period as f32)
        .collect()
}

/// Returns the sum of all task's weight
///
/// # Arguments
///
/// `tasks_stats`: tasks scheduling statistics
/// `period`: an imaginary time period that is assumed to be fairly shared among
///  the tasks in nanoseconds
///
fn get_tasks_weight_sum(tasks_stats: &Vec<TaskStatistics>) -> i64 {
    tasks_stats
        .iter()
        .fold(0i64, |acc, t| acc + scale_prio_to_weight(t.prio) as i64)
}

/// Returns the sum of all normalized task's cpu time
fn get_tasks_normalized_cpu_time_sum(tasks_stats: &Vec<TaskStatistics>, period: u64) -> f32 {
    tasks_stats.iter().fold(0f32, |acc, t| {
        acc + t.calculate_cpu_fair_share_per_period_ns(period)
    })
}

/// Returns the counts of each task states enums
pub fn get_all_tasks_states_count(tasks_stats: &Vec<TaskStatistics>) -> AllTasksStatesCounts {
    let mut tasks_states_counts = AllTasksStatesCounts::default();
    for t in tasks_stats.iter() {
        match t.last_seen_state {
            s if s == *TASK_RUNNING_RQ => tasks_states_counts.num_tasks_running_rq += 1,
            s if s == *TASK_RUNNING_CPU => tasks_states_counts.num_tasks_running_cpu += 1,
            s if s == *TASK_WAITING => tasks_states_counts.num_tasks_waiting += 1,
            s if s == *__TASK_STOPPED => tasks_states_counts.num_tasks_stopped += 1,
            _ => tasks_states_counts.num_tasks_unknown_state += 1,
        }
    }

    tasks_states_counts
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
pub fn filter_tasks(tasks_stats: Vec<TaskStatistics>, app: &App) -> Vec<TaskStatistics> {
    tasks_stats
        .into_iter()
        .filter(|t| t.nr_switches >= app.min_nr_switches && t.prio >= 100 && t.prio <= 139)
        .collect()
}

/// Calculates the actual tasks priority based on their used fair shares
///
///
pub fn calculate_tasks_actual_fair_share_prio(
    actual_fair_shares: &Vec<f32>,
    ideal_fair_shares: &Vec<f32>,
) -> Vec<i16> {
    let scaling = 1.25;
    let mut delta_prio = Vec::new();

    for i in 0..actual_fair_shares.len() {
        let actual = actual_fair_shares[i];
        let ideal = ideal_fair_shares[i];

        let log_actual = actual.log(scaling);
        let log_ideal = ideal.log(scaling);
        let log_scaling = scaling.log(scaling);

        let k = (log_actual - log_ideal) / log_scaling;
        let clamped_k = k.max(-20.0).min(19.0);
        let rounded_k = clamped_k.round() as i16;
        delta_prio.push(rounded_k);
    }

    delta_prio
}
