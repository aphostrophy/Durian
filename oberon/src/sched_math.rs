#[cfg(test)]
mod tests;

#[rustfmt::skip]
const SCHED_PRIO_TO_WEIGHT: [i32; 40] = [
    /* -20 */ 88761, 71755, 56483, 46273, 36291, 
    /* -15 */ 29154, 23254, 18705, 14949, 11916, 
    /* -10 */ 9548,  7620,  6100,  4904,  3906, 
    /*  -5 */ 3121,  2501,  1991,  1586,  1277, 
    /*   0 */ 1024,  820,   655,   526,   423, 
    /*   5 */ 335,   272,   215,   172,   137, 
    /*  10 */ 110,   87,    70,    56,    45, 
    /*  15 */ 36,    29,    23,    18,    15, 
    ];

/// Calculates the weighting of a task based on its process priority.
///
/// Maps task (fair) priority from priority space to nice value space.
/// It then maps the nice values to its corresponding weightings based
/// on SCHED_PRIO_TO_WEIGHT. Task priority range is from 100 to 139 inclusive.
///
/// # Note
/// To clear some misunderstandings it is worth noting that "higher priority" process
/// doesn't always refer to higher valued priority. It depends on the context of "priority".
/// Sometimes we talk priority in terms of better interactiveness and sometimes it's about
/// more CPU time allocated for a task. In the Linux Kernel, at least for tasks scheduled with
/// CFS, higher priority means better interactiveness so the value has a positive correlation
/// with nice values. This also results in lower CPU fair share for higher priority processes.
///
/// # Arguments
///
/// `prio` - task priority (100 <= prio <= 139)
///
/// # Examples
///
/// . . .
pub fn scale_prio_to_weight(prio: i16) -> i32 {
    let nice = prio - 120;
    let offset = 20;

    SCHED_PRIO_TO_WEIGHT[(nice + offset) as usize]
}

/// Calculates the task's nice value based on kernel priority.
///
/// Maps task (fair) priority from priority space to nice value space.
/// Task (fair) priority ranges from 100-139 which is directly mapped
/// into nice values which ranges from -20 to +19.
///
/// Usage of common linux commands e.g. 'top' might show 0-39 instead of 100-139.
/// Mapping between 0-39 to 100-139 is trivial and direct just add +100.
///
/// # Arguments
///
/// `prio` - task priority (100 <= prio <= 139)
/// # Examples
///
/// . . .
pub fn prio_to_nice(prio: i16) -> i16 {
    prio - 120
}

/// Calculates the task's priority value based on nice value
///
/// Inverse of prio_to_nice
///
/// # Arguments
///
/// `prio` - task priority (100 <= prio <= 139)
/// # Examples
///
/// . . .
pub fn nice_to_prio(nice: i16) -> i16 {
    nice + 120
}

pub fn duration_ns_to_fmt_duration(duration_ns: u64) -> String {
    let mut remaining_seconds = duration_ns / 1_000_000_000;
    let days = remaining_seconds / (60 * 60 * 24);
    remaining_seconds = remaining_seconds % (60 * 60 * 24);
    let hours = remaining_seconds / (60 * 60);
    remaining_seconds = remaining_seconds % (60 * 60);
    let minutes = remaining_seconds / 60;
    remaining_seconds = remaining_seconds % 60;
    let seconds = remaining_seconds;

    format!("{:02}-{:02}:{:02}:{:02}", days, hours, minutes, seconds)
}
