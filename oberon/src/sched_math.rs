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
/// on SCHED_PRIO_TO_WEIGHT. Task priority range is from 100 to 139 inclusive
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
