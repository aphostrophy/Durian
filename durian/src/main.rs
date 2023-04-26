use structopt::StructOpt;

mod app;
mod client;
mod config;
mod core;
mod directories;
mod durian_def;
mod errors;
mod models;
mod repository;
mod sched_math;

use errors::DurianResult;

fn main() -> DurianResult<()> {
    let mut app = app::App::from_args();
    app.validate()?;
    app.process_config_file();

    let mut client = client::Client::new();

    let _resp = client.perform_action(&app, &app.cmd)?;
    Ok(())
}
