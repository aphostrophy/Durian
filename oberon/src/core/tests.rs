use crate::core::{
    get_tasks_average_cpu_time, get_tasks_average_io_time, get_tasks_normalized_cpu_fair_share_ns,
    get_tasks_weight_sum,
};
use crate::models::task_statistics::TaskStatistics;

#[test]
fn get_tasks_average_io_time_standard_non_zero() {
    let tasks_stats = vec![
        TaskStatistics::new(0, "A".to_string(), 120, 33000, 130000, 0, 99999999, 0, 30),
        TaskStatistics::new(0, "B".to_string(), 120, 66000, 130000, 0, 99999999, 0, 45),
    ];
    let avg = get_tasks_average_io_time(&tasks_stats);
    assert_eq!(avg, 49500.0);
}

#[test]
fn get_tasks_average_io_time_empty_zero() {
    let tasks_stats = vec![];
    let avg = get_tasks_average_io_time(&tasks_stats);
    assert_eq!(avg, 0.0);
}

#[test]
fn get_tasks_average_cpu_time_standard_non_zero() {
    let tasks_stats = vec![
        TaskStatistics::new(0, "A".to_string(), 120, 33000, 260000, 0, 99999999, 0, 30),
        TaskStatistics::new(0, "B".to_string(), 120, 66000, 130000, 0, 99999999, 0, 45),
    ];
    let avg = get_tasks_average_cpu_time(&tasks_stats);
    assert_eq!(avg, 195000.0);
}

#[test]
fn get_tasks_average_cpu_time_empty_zero() {
    let tasks_stats = vec![];
    let avg = get_tasks_average_cpu_time(&tasks_stats);
    assert_eq!(avg, 0.0);
}

#[test]
fn get_tasks_normalized_cpu_fair_share_ns_empty_case() {
    let period = 20_000_000;
    let tasks_stats = vec![];
    let tasks_normalized_cpu_fair_share_ns =
        get_tasks_normalized_cpu_fair_share_ns(&tasks_stats, period);
    assert_eq!(tasks_normalized_cpu_fair_share_ns, vec![]);
}

#[test]
fn get_tasks_normalized_cpu_fair_share_ns_equal_total_cpu() {
    let period = 30_000_000;
    let tasks_stats = vec![
        TaskStatistics::new(
            0,
            "A".to_string(),
            120,
            33000,
            130000,
            0,
            260000,
            130000,
            30,
        ),
        TaskStatistics::new(
            1,
            "B".to_string(),
            120,
            66000,
            130000,
            0,
            130000,
            000000,
            45,
        ),
        TaskStatistics::new(
            2,
            "C".to_string(),
            120,
            66000,
            130000,
            0,
            390000,
            260000,
            45,
        ),
    ];
    let tasks_normalized_cpu_fair_share_ns =
        get_tasks_normalized_cpu_fair_share_ns(&tasks_stats, period);
    assert_eq!(
        tasks_normalized_cpu_fair_share_ns,
        vec![10_000_000.0, 10_000_000.0, 10_000_000.0]
    );
}

#[test]
fn get_tasks_normalized_cpu_fair_share_ns_unequal_total_cpu() {
    let period = 20_000_000;
    let tasks_stats = vec![
        TaskStatistics::new(0, "A".to_string(), 120, 33000, 50000, 0, 200000, 100000, 30),
        TaskStatistics::new(
            1,
            "B".to_string(),
            120,
            66000,
            100000,
            0,
            300000,
            000000,
            45,
        ),
    ];
    let tasks_normalized_cpu_fair_share_ns =
        get_tasks_normalized_cpu_fair_share_ns(&tasks_stats, period);
    assert_eq!(
        tasks_normalized_cpu_fair_share_ns,
        vec![12_000_000.0, 8_000_000.0]
    );
}

#[test]
fn get_tasks_weight_sum_empty_case() {
    let tasks_stats = vec![];
    let weight_sum = get_tasks_weight_sum(&tasks_stats);
    assert_eq!(weight_sum, 0);
}

#[test]
fn get_tasks_weight_sum_multiple_tasks() {
    let tasks_stats = vec![
        TaskStatistics::new(0, "A".to_string(), 100, 33000, 260000, 0, 99999999, 0, 30),
        TaskStatistics::new(0, "B".to_string(), 120, 66000, 130000, 0, 99999999, 0, 45),
    ];
    let weight_sum = get_tasks_weight_sum(&tasks_stats);
    assert_eq!(weight_sum, 89785);
}
