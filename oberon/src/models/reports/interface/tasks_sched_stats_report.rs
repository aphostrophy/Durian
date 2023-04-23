#[typetag::serde(tag = "driver")]
pub trait TasksSchedStatsReport: std::fmt::Debug {
    fn report(&self, filename: &str) -> std::io::Result<()>;
}
