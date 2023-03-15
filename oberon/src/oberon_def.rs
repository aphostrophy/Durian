mod c {
    use std::ffi::c_int;

    #[link(name = "oberon_def", kind = "static")]
    extern "C" {
        pub static ITASK_RUNNING_RQ: c_int;
        pub static ITASK_RUNNING_CPU: c_int;
        pub static ITASK_WAITING: c_int;
        pub static I__TASK_STOPPED: c_int;
    }
}

pub static TASK_RUNNING_RQ: &i32 = unsafe { &c::ITASK_RUNNING_RQ };
pub static TASK_RUNNING_CPU: &i32 = unsafe { &c::ITASK_RUNNING_CPU };
pub static TASK_WAITING: &i32 = unsafe { &c::ITASK_WAITING };
pub static __TASK_STOPPED: &i32 = unsafe { &c::I__TASK_STOPPED };
