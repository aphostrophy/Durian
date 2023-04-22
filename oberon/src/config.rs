use serde::{Deserialize, Serialize};
use std::fs;
use std::path::PathBuf;

use crate::app::App;
use crate::directories::DIRECTORIES;

#[derive(Debug, Deserialize, Serialize)]
pub struct Config {
    pub sched_min_granularity_ns: Option<u64>,
    pub sched_latency_ns: Option<u64>,
    pub min_nr_switches: Option<u32>,
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

impl Config {
    pub fn read_config_from_app(app: &App) -> Config {
        Config {
            min_nr_switches: Some(app.min_nr_switches),
            sched_latency_ns: Some(app.sched_latency_ns),
            sched_min_granularity_ns: Some(app.sched_min_granularity_ns),
        }
    }
}

impl std::fmt::Display for Config {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        if let Some(min_nr_switches) = self.min_nr_switches {
            writeln!(f, "min_nr_switches: {}", min_nr_switches)?;
        } else {
            writeln!(f, "min_nr_switches: not specified")?;
        }

        if let Some(sched_latency_ns) = self.sched_latency_ns {
            writeln!(f, "sched_latency_ns: {} ns", sched_latency_ns)?;
        } else {
            writeln!(f, "sched_latency_ns: not specified")?;
        }

        if let Some(sched_min_granularity_ns) = self.sched_min_granularity_ns {
            writeln!(
                f,
                "sched_min_granularity_ns: {} ns",
                sched_min_granularity_ns
            )?;
        } else {
            writeln!(f, "sched_min_granularity_ns: not specified")?;
        }

        Ok(())
    }
}
