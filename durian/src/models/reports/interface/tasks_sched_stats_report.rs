use crate::app::App;

#[typetag::serde(tag = "driver")]
pub trait TasksSchedStatsReport: std::fmt::Debug {
    fn report(&self, filename: &str, app: &App) -> std::io::Result<()>;
}
