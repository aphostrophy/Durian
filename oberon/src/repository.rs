use std::collections::HashSet;

use redis::{self, Commands};

use crate::{errors::OberonResult, oberon_def::running_pid_set_key};

pub fn fetch_active_tasks(conn: &mut redis::Connection) -> OberonResult<HashSet<i32>> {
    let set_key = running_pid_set_key()?;
    let active_tasks: HashSet<i32> = conn.smembers(set_key)?;
    Ok(active_tasks)
}
