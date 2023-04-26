use std::collections::HashSet;

use redis::{self, Commands};

use crate::durian_def::running_pid_set_key;
use crate::errors::DurianResult;
use crate::models::task_statistics::TaskStatistics;

pub fn gen_all_tasks_complete_statistics(
    conn: &mut redis::Connection,
) -> DurianResult<Vec<TaskStatistics>> {
    let active_tasks_pid = fetch_active_tasks(conn)?;
    let mut tasks_statistics = fetch_tasks_statistics(conn, active_tasks_pid)?;
    tasks_statistics.sort_by_key(|t| t.pid);

    Ok(tasks_statistics)
}

pub fn gen_task_complete_statistics(
    conn: &mut redis::Connection,
    pid: &i32,
) -> DurianResult<TaskStatistics> {
    let task_statistics = fetch_task_statistics(conn, *pid)?;
    Ok(task_statistics)
}

// Fetch all active tasks pids excluding 0 (init process)
pub fn fetch_active_tasks(conn: &mut redis::Connection) -> DurianResult<HashSet<i32>> {
    let set_key = running_pid_set_key()?;
    let mut active_tasks: HashSet<i32> = conn.smembers(set_key)?;
    active_tasks.remove(&0);

    Ok(active_tasks)
}

pub fn fetch_tasks_statistics(
    conn: &mut redis::Connection,
    pids: HashSet<i32>,
) -> DurianResult<Vec<TaskStatistics>> {
    let stats = pids
        .iter()
        .map(|pid| fetch_task_statistics(conn, *pid))
        .collect::<Result<Vec<_>, _>>()?;

    Ok(stats)
}

pub fn fetch_task_statistics(
    conn: &mut redis::Connection,
    pid: i32,
) -> DurianResult<TaskStatistics> {
    let keys = vec![
        format!("{}:prio", pid),
        format!("{}:comm", pid),
        format!("{}:total_wait_time_ns", pid),
        format!("{}:total_cpu_time_ns", pid),
        format!("{}:last_seen_state", pid),
        format!("{}:last_ktime_ns", pid),
        format!("{}:sched_stats_start_time_ns", pid),
        format!("{}:nr_switches", pid),
    ];
    let values: Vec<String> = conn.mget(keys)?;

    let prio = values[0].parse::<i16>()?;
    let comm = values[1].clone();
    let total_wait_time_ns = values[2].parse::<u64>()?;
    let total_cpu_time_ns = values[3].parse::<u64>()?;
    let last_seen_state = values[4].parse::<u8>()?;
    let last_ktime_ns = values[5].parse::<u64>()?;
    let sched_stats_start_time_ns = values[6].parse::<u64>()?;
    let nr_switches = values[7].parse::<u32>()?;

    Ok(TaskStatistics::new(
        pid,
        comm,
        prio,
        total_wait_time_ns,
        total_cpu_time_ns,
        last_seen_state,
        last_ktime_ns,
        sched_stats_start_time_ns,
        nr_switches,
    ))
}
