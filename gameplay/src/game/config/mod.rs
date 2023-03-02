mod configs;
use std::collections::HashMap;

pub use configs::*;
use once_cell::sync::OnceCell;
use shared::{boxed::MutableBox, GameResult};
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ConfigType{
    SkillConfig,
}
static SKILL_CONFIGS: OnceCell<MutableBox<HashMap<u32, SkillConfig>>> = OnceCell::new();
pub fn get_skill_config(id: u32) -> Option<SkillConfig>{
    SKILL_CONFIGS.get().expect("skill config not initialized").lock_ref().unwrap().get(&id).cloned()
}
pub fn load_configs(path: &str) -> GameResult<()>{
    let mut skills = SKILL_CONFIGS.get_or_init(|| MutableBox::new(HashMap::new())).lock_mut().unwrap();
    //TODO
    skills.insert(1, SkillConfig{
        id: 1,
    });
    skills.insert(2, SkillConfig{
        id: 2,
    });
    skills.insert(3, SkillConfig{
        id: 3,
    });
    Ok(())
}
pub fn unload(){
    SKILL_CONFIGS.get().map(|configs| configs.lock_mut().unwrap().clear());
}