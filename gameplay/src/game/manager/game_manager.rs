//! game manager

use super::{CharacterManager, CreatureManager, SchedulerManager};
pub struct GameManager{
    //TODO
}
impl GameManager {
    pub fn update(delta: f32){
        SchedulerManager::update(delta);
        super::combat_manager().update(delta);
        CharacterManager::update(delta);
        CreatureManager::update(delta);
    }
}