use serde::{Deserialize, Serialize};

#[derive(Debug, Serialize, Deserialize)]
pub struct TaskStatistics {
    pub pid: i32,
    pub comm: String,
    pub prio: i16,
    pub total_wait_time_ns: u64,
    pub total_cpu_time_ns: u64,
    pub last_seen_state: u8,
    pub last_ktime_ns: u64,
    pub sched_stats_start_time_ns: u64,
    pub nr_switches: u32,
    pub avg_cpu_timeslice: f32, // average cpu time proportion used for every context switch
}

impl TaskStatistics {
    pub fn new(
        pid: i32,
        comm: String,
        prio: i16,
        total_wait_time_ns: u64,
        total_cpu_time_ns: u64,
        last_seen_state: u8,
        last_ktime_ns: u64,
        sched_stats_start_time_ns: u64,
        nr_switches: u32,
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

    /// Returns the CPU fair share based on existing task statistics.
    ///
    /// The timeslice used by a task for each time period assuming
    /// task's timeslice distribution is uniform in respect to
    /// time.
    ///
    /// # Arguments
    ///
    /// `period`: Time period where each task in the run queue are assumed
    ///           to have been scheduled at least once, sched_latency_ns value
    ///           can be used.
    pub fn calculate_cpu_fair_share_per_period_ns(&self, period: u64) -> f32 {
        let proportion =
            period as f32 / (self.last_ktime_ns - self.sched_stats_start_time_ns) as f32;

        let normalized_cpu_time_ns = proportion * self.total_cpu_time_ns as f32;
        normalized_cpu_time_ns
    }
}
