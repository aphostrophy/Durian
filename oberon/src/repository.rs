use std::collections::HashSet;

use redis::{self, Commands};

use crate::errors::OberonResult;
use crate::models::TaskStatistics;
use crate::oberon_def::running_pid_set_key;

pub fn fetch_active_tasks(conn: &mut redis::Connection) -> OberonResult<HashSet<i32>> {
    let set_key = running_pid_set_key()?;
    let active_tasks: HashSet<i32> = conn.smembers(set_key)?;
    Ok(active_tasks)
}

pub fn fetch_tasks_statistics(
    conn: &mut redis::Connection,
    pids: HashSet<i32>,
) -> OberonResult<Vec<TaskStatistics>> {
    let stats = pids
        .iter()
        .map(|pid| fetch_task_statistics(conn, *pid))
        .collect::<Result<Vec<_>, _>>()?;

    Ok(stats)
}

pub fn fetch_task_statistics(
    conn: &mut redis::Connection,
    pid: i32,
) -> OberonResult<TaskStatistics> {
    let keys = vec![
        format!("{}:prio", pid),
        format!("{}:comm", pid),
        format!("{}:total_wait_time_ns", pid),
        format!("{}:total_cpu_time_ns", pid),
        format!("{}:last_seen_state", pid),
        format!("{}:last_ktime_ns", pid),
    ];
    let values: Vec<String> = conn.mget(keys)?;

    let prio = values[0].parse::<i8>()?;
    let comm = values[1].clone();
    let total_wait_time_ns = values[2].parse::<i64>()?;
    let total_cpu_time_ns = values[3].parse::<i64>()?;
    let last_seen_state = values[4].parse::<i8>()?;
    let last_ktime_ns = values[5].parse::<i64>()?;

    Ok(TaskStatistics {
        pid,
        comm,
        prio,
        total_wait_time_ns,
        total_cpu_time_ns,
        last_seen_state,
        last_ktime_ns,
    })
}
