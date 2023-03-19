use std::{ffi::CStr, str::Utf8Error};

mod c {
    use std::ffi::{c_char, c_int};

    #[link(name = "oberon_def", kind = "static")]
    extern "C" {
        pub static ITASK_RUNNING_RQ: c_int;
        pub static ITASK_RUNNING_CPU: c_int;
        pub static ITASK_WAITING: c_int;
        pub static I__TASK_STOPPED: c_int;
        pub static IRUNNING_PID_SET: *const c_char;
    }
}

pub fn running_pid_set_key() -> Result<&'static str, Utf8Error> {
    let char_ptr = unsafe { c::IRUNNING_PID_SET };
    let c_str = unsafe { CStr::from_ptr(char_ptr) };
    c_str.to_str()
}

pub static TASK_RUNNING_RQ: &i32 = unsafe { &c::ITASK_RUNNING_RQ };
pub static TASK_RUNNING_CPU: &i32 = unsafe { &c::ITASK_RUNNING_CPU };
pub static TASK_WAITING: &i32 = unsafe { &c::ITASK_WAITING };
pub static __TASK_STOPPED: &i32 = unsafe { &c::I__TASK_STOPPED };
