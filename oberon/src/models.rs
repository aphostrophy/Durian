#[derive(Debug)]
pub struct TaskStatistics {
    pub pid: i32,
    pub comm: String,
    pub prio: i8,
    pub total_wait_time_ns: i64,
    pub total_cpu_time_ns: i64,
    pub last_seen_state: i8,
    pub last_ktime_ns: i64,
}

pub trait TasksSchedStatReport {
    fn dump(path: Option<&str>);
    fn display();
    fn load(path: Option<&str>);
}
