use structopt::StructOpt;

use crate::errors::OberonResult;

#[derive(StructOpt, Debug)]
#[structopt(name = "oberon")]
pub struct App {
    #[structopt(short, long)]
    pub quiet: bool,

    #[structopt(subcommand)]
    pub cmd: Command,
}

impl App {
    pub fn validate(&mut self) -> OberonResult<()> {
        Ok(())
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
