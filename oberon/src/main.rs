use structopt::StructOpt;

mod app;
mod client;
mod config;
mod core;
mod directories;
mod errors;
mod models;
mod oberon_def;
mod repository;
mod sched_math;

use errors::OberonResult;

fn main() -> OberonResult<()> {
    let mut app = app::App::from_args();
    app.validate()?;
    app.process_config_file();

    let mut client = client::Client::new();

    let _resp = client.perform_action(&app, &app.cmd)?;
    Ok(())
}
