use serde::{Deserialize, Serialize};

#[derive(Debug, Serialize, Deserialize)]
pub struct TaskStatistics {
    pub pid: i32,
    pub comm: String,
    pub prio: i16,
    pub total_wait_time_ns: i64,
    pub total_cpu_time_ns: i64,
    pub last_seen_state: i8,
    pub last_ktime_ns: i64,
    pub sched_stats_start_time_ns: i64,
    pub nr_switches: i32,
    pub avg_cpu_timeslice: f32, // average cpu time proportion used for every context switch
}

impl TaskStatistics {
    pub fn new(
        pid: i32,
        comm: String,
        prio: i16,
        total_wait_time_ns: i64,
        total_cpu_time_ns: i64,
        last_seen_state: i8,
        last_ktime_ns: i64,
        sched_stats_start_time_ns: i64,
        nr_switches: i32,
    ) -> Self {
        let avg_cpu_timeslice = total_cpu_time_ns as f32 / nr_switches as f32;
        TaskStatistics {
            pid,
            comm,
            prio,
            total_wait_time_ns,
            total_cpu_time_ns,
            last_seen_state,
            last_ktime_ns,
            sched_stats_start_time_ns,
            nr_switches,
            avg_cpu_timeslice,
        }
    }
}

#[typetag::serde(tag = "driver")]
pub trait TasksSchedStatsReport: std::fmt::Debug {}

#[derive(Debug, Serialize, Deserialize)]
pub struct AllTasksCompleteStatsReport {
    pub num_tasks: usize,
    pub avg_io_time_ns: f32,
    pub avg_cpu_time_ns: f32,
    pub tasks_stats: Vec<TaskStatistics>,
}

#[typetag::serde(name = "all_tasks_complete_stats_report")]
impl TasksSchedStatsReport for AllTasksCompleteStatsReport {}

#[derive(Debug, Serialize, Deserialize)]
pub struct TaskCompleteStatsReport {
    pub task_stats: TaskStatistics,
}

#[typetag::serde(name = "task_complete_stats_report")]
impl TasksSchedStatsReport for TaskCompleteStatsReport {}
