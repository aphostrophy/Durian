use crate::sched_math::{nice_to_prio, prio_to_nice};

#[test]
fn test_nice_to_prio_default_prio() {
    let prio = nice_to_prio(0);
    assert_eq!(prio, 120);
}

#[test]
fn test_nice_to_prio_high() {
    let prio = nice_to_prio(18);
    assert_eq!(prio, 138);
}

#[test]
fn test_nice_to_prio_low() {
    let prio = nice_to_prio(-15);
    assert_eq!(prio, 105);
}

#[test]
fn test_prio_to_nice_default_nice() {
    let nice = prio_to_nice(120);
    assert_eq!(nice, 0);
}

#[test]
fn test_prio_to_nice_high() {
    let nice = prio_to_nice(125);
    assert_eq!(nice, 5);
}

#[test]
fn test_prio_to_nice_low() {
    let nice = prio_to_nice(113);
    assert_eq!(nice, -7);
}
