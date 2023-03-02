pub trait IConfig{
    fn get_id(&self) -> u32;
}
mod skill_config;
pub use skill_config::SkillConfig;
