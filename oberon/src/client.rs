use crate::app::{App, Command, ShowCommand};
use crate::errors::OberonResult;
use crate::models::TaskStatistics;
use crate::repository;

pub struct Client {
    repository: redis::Connection,
}

impl Client {
    pub fn new() -> Self {
        let redis_client = redis::Client::open("redis://127.0.0.1/");
        let con = match redis_client {
            Ok(redis_client) => redis_client.get_connection(),
            Err(err) => panic!("Failed to connect to Redis: {}", err),
        };

        match con {
            Ok(con) => Client { repository: con },
            Err(err) => panic!("Failed on opening Redis connection: {}", err),
        }
    }

    pub fn perform_action(&mut self, app: &App, action: &Command) -> OberonResult<()> {
        match action {
            Command::SHOW { show_command } => self.perform_show(app, show_command),
        }
    }

    pub fn perform_show(
        &mut self,
        _app: &App,
        show_command: &Option<ShowCommand>,
    ) -> OberonResult<()> {
        match show_command {
            Some(ref command) => match command {
                ShowCommand::All(options) => match options.pid {
                    Some(ref pid) => gen_task_complete_statistics(&mut self.repository, pid),
                    None => {
                        gen_all_tasks_complete_statistics(&mut self.repository)?;
                        Ok(())
                    }
                },
            },
            None => {
                gen_all_tasks_complete_statistics(&mut self.repository)?;
                Ok(())
            }
        }
    }
}

pub fn gen_all_tasks_complete_statistics(
    conn: &mut redis::Connection,
) -> OberonResult<Vec<TaskStatistics>> {
    let active_tasks_pid = repository::fetch_active_tasks(conn)?;
    let tasks_statistics = repository::fetch_tasks_statistics(conn, active_tasks_pid)?;
    Ok(tasks_statistics)
}

pub fn gen_task_complete_statistics(_conn: &mut redis::Connection, _pid: &i32) -> OberonResult<()> {
    Ok(())
}