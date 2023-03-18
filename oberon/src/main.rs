use structopt::StructOpt;

mod app;
mod client;
mod errors;
mod oberon_def;

use errors::OberonResult;

fn main() -> OberonResult<()> {
    let mut app = app::App::from_args();
    app.validate()?;

    let _resp = client::perform_action(&app, &app.cmd)?;
    Ok(())
}
