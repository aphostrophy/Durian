use serde::Deserialize;
use std::fs;
use std::path::PathBuf;

use crate::app::App;
use crate::directories::DIRECTORIES;

#[derive(Debug, Deserialize)]
pub struct Config {
    pub sched_min_granularity_ns: Option<i64>,
    pub sched_latency_ns: Option<i64>,
    pub min_nr_switches: Option<i64>,
}

pub fn config_file(app: &App) -> PathBuf {
    app.config
        .as_ref()
        .cloned()
        .filter(|config_path| config_path.is_file())
        .unwrap_or_else(|| DIRECTORIES.config().join("config"))
}

pub fn read_config_file(path: PathBuf) -> Option<Config> {
    fs::read_to_string(path).ok().map(|content| {
        let config: Config = toml::from_str(&content).unwrap();
        config
    })
}
