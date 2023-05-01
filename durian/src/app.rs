use std::path::PathBuf;
use structopt::StructOpt;

use crate::config;
use crate::errors::DurianResult;

#[derive(StructOpt, Debug)]
#[structopt(name = "durian")]
pub struct App {
    #[structopt(short, long)]
    pub quiet: bool,

    /// The minimum number of context switch a Task needs to do before it's
    /// even considered by Durian. If nr_switches < min_nr_switches then
    /// Durian will ignore that task from any processing.
    ///
    /// This is used if we want to ignore short-lived processes as it
    /// might cause some bias towards calculation. We might also not
    /// care that much of short lived processes as most of the time it doesn't
    /// need optimization and it's harder to keep track of (same tasks
    /// with different pids)
    ///
    /// e.g. Normalized CPU fair share of a newly run task might make it seem
    /// like the task is dominating the CPU.
    ///
    #[structopt(short, long, default_value = "0")]
    pub min_nr_switches: u32,

    /// In the kernel this is known as the lower bound for the minimum time a task has to execute even
    /// if there are other tasks with lower vruntime. Hence any task will need to run at least for
    /// sched_min_granularity_ns before being pre-empted out (it can however, still yield() itself).
    #[structopt(long, default_value = "0")]
    pub sched_min_granularity_ns: u64,

    /// In the kernel this is known as the scheduler period which is the period
    /// in which all run queue tasks are scheduled at least once.
    ///
    /// Here, it's used as an imaginary period where all tasks are scheduled k number of times.
    /// Calculation regarding normalized_cpu_fair_share will use this number. See core.rs for further details.
    #[structopt(long, default_value = "0")]
    pub sched_latency_ns: u64,

    #[structopt(subcommand)]
    pub cmd: Command,

    /// Configuration file.
    ///
    /// A TOML file which is stored by default at HOME/.config/durian/config
    /// where HOME is platform dependent.
    ///
    /// The file supports the following optional keys with the given types:
    /// sched_min_granularity_ns: u64
    /// sched_latency_ns: u64
    /// min_nr_switches: u32
    ///
    /// Each option has the same meaning as the corresponding configuration
    /// option with the same name.
    #[structopt(short, long, env = "DURIAN_CONFIG", parse(from_os_str))]
    pub config: Option<PathBuf>,
}

impl App {
    pub fn validate(&mut self) -> DurianResult<()> {
        Ok(())
    }

    pub fn process_config_file(&mut self) {
        let config_path = config::config_file(self);
        let config_opt = config::read_config_file(config_path);
        if let Some(config) = config_opt {
            if self.min_nr_switches == 0 {
                if let Some(m) = config.min_nr_switches {
                    self.min_nr_switches = m;
                }
            }

            if self.sched_min_granularity_ns == 0 {
                if let Some(s) = config.sched_min_granularity_ns {
                    self.sched_min_granularity_ns = s;
                }
            }

            if self.sched_latency_ns == 0 {
                if let Some(s) = config.sched_latency_ns {
                    self.sched_latency_ns = s;
                }
            }
        }
    }
}

#[derive(StructOpt, Debug)]
pub enum Command {
    #[structopt(name = "record")]
    RECORD {
        #[structopt(subcommand)]
        record_command: Option<RecordCommand>,
    },
    #[structopt(name = "report")]
    REPORT {
        #[structopt(subcommand)]
        report_command: Option<ReportCommand>,
    },
}

#[derive(StructOpt, Debug)]
pub enum RecordCommand {
    All(RecordCommandOptions),
}

#[derive(StructOpt, Debug)]
pub struct RecordCommandOptions {
    pub pid: Option<i32>,
}

#[derive(StructOpt, Debug)]
pub enum ReportCommand {
    All(ReportCommandOptions),
}

// All ReportCommand Enums should be of this type
#[derive(StructOpt, Debug)]
pub struct ReportCommandOptions {
    pub path: Option<String>,

    #[structopt(short, long, default_value = "durian.data")]
    pub output_path: String,
}
