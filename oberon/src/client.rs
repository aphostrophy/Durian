use crate::app::{App, Command, ShowCommand};
use crate::errors::{Error, OberonResult};

pub fn perform_action(app: &App, action: &Command) -> OberonResult<()> {
    match action {
        Command::SHOW { show_command } => perform_show(app, show_command),
    }
}

pub fn perform_show(app: &App, show_command: &Option<ShowCommand>) -> OberonResult<()> {
    match show_command {
        Some(ref command) => match command {
            ShowCommand::All(options) => match options.pid {
                Some(ref pid) => {
                    println!("[COMMAND] SHOW {}", pid);
                    Ok(())
                }
                None => {
                    println!("[COMMAND] SHOW!");
                    Ok(())
                }
            },
        },
        None => {
            println!("[COMMAND] SHOW DEFAULT");
            Ok(())
        }
    }
}
