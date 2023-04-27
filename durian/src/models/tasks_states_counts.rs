use serde::{Deserialize, Serialize};

/// Represents the number of undeleted tasks tracked by Durian
///
/// Size of num_tasks is u32 to handle possibility of a non default allowed
/// maximum num procs (pid > 65535)
#[derive(Debug, Serialize, Deserialize, Default)]
pub struct AllTasksStatesCounts {
    pub num_tasks_running_rq: u32,
    pub num_tasks_running_cpu: u32,
    pub num_tasks_waiting: u32,
    pub num_tasks_stopped: u32,

    pub num_tasks_unknown_state: u32,
}
