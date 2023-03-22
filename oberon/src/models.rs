#[derive(Debug)]
pub struct TaskStatistics {
    pub pid: i32,
    pub comm: String,
    pub prio: i16,
    pub total_wait_time_ns: i64,
    pub total_cpu_time_ns: i64,
    pub last_seen_state: i8,
    pub last_ktime_ns: i64,
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
            nr_switches,
            avg_cpu_timeslice,
        }
    }
}

pub trait TasksSchedStatsReport {
    fn dump(path: Option<&str>);
    fn display();
    fn load(path: Option<&str>);
}

pub struct AllTasksCompleteStatsReport {
    pub num_tasks: f32,
    pub avg_io_time_ns: f32,
    pub avg_cpu_time_ns: f32,
    pub tasks_stats: Vec<TaskStatistics>,
}
