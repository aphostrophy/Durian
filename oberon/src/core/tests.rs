use crate::core::{
    get_tasks_average_cpu_time, get_tasks_average_io_time, get_tasks_normalized_cpu_fair_share_ns,
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
