mod oberon_def;

fn main() {
    println!("TASK_RUNNING_RQ = {}", { oberon_def::TASK_RUNNING_RQ });
    println!("TASK_RUNNING_CPU = {}", { oberon_def::TASK_RUNNING_CPU });
    println!("TASK_WAITING = {}", { oberon_def::TASK_WAITING });
    println!("__TASK_STOPPED = {}", { oberon_def::__TASK_STOPPED });
}
