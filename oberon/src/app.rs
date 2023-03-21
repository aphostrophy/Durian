use std::path::PathBuf;
use structopt::StructOpt;

use crate::config;
use crate::errors::OberonResult;

#[derive(StructOpt, Debug)]
#[structopt(name = "oberon")]
pub struct App {
    #[structopt(short, long)]
    pub quiet: bool,

    #[structopt(subcommand)]
    pub cmd: Command,

    pub config: Option<PathBuf>,
}

impl App {
    pub fn validate(&mut self) -> OberonResult<()> {
        Ok(())
    }

    pub fn process_config_file(&mut self) {
        let config_path = config::config_file(self);
        let config_opt = config::read_config_file(config_path);
        if let Some(mut config) = config_opt {
            // Use config here
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
