use std::path::PathBuf;
use structopt::StructOpt;

use crate::config;
use crate::errors::OberonResult;

#[derive(StructOpt, Debug)]
#[structopt(name = "oberon")]
pub struct App {
    #[structopt(short, long)]
    pub quiet: bool,

    #[structopt(short, long, default_value = "0")]
    pub min_nr_switches: u32,

    #[structopt(long, default_value = "0")]
    pub sched_min_granularity_ns: u64,

    #[structopt(long, default_value = "0")]
    pub sched_latency_ns: u64,

    #[structopt(subcommand)]
    pub cmd: Command,

    /// Configuration file.
    ///
    /// A TOML file which is stored by default at HOME/.config/oberon/config
    /// where HOME is platform dependent.
    ///
    /// The file supports the following optional keys with the given types:
    /// sched_min_granularity_ns: u64
    /// sched_latency_ns: u64
    /// min_nr_switches: u32
    ///
    /// Each option has the same meaning as the corresponding configuration
    /// option with the same name.
    #[structopt(short, long, env = "OBERON_CONFIG", parse(from_os_str))]
    pub config: Option<PathBuf>,
}

impl App {
    pub fn validate(&mut self) -> OberonResult<()> {
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
    #[structopt(name = "show")]
    SHOW {
        #[structopt(subcommand)]
        show_command: Option<ShowCommand>,
    },
}

#[derive(StructOpt, Debug)]
pub enum ShowCommand {
    All(ShowCommandOptions),
}

#[derive(StructOpt, Debug)]
pub struct ShowCommandOptions {
    pub pid: Option<i32>,
}
