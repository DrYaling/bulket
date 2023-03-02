//! game managers
//! 
mod player_manager;
mod character_manager;
mod creature_manager;
mod object_manager;
mod combat_manager;
mod input_manager;
mod game_manager;
mod scheduler;
pub mod anim_notify_manager;
use engine_api::{ActorSpawnedEvent, SpawnObject, ActorDestroyEvent};
pub use player_manager::*;
pub use character_manager::*;
pub use creature_manager::*;
pub use object_manager::*;
pub use combat_manager::*;
pub use input_manager::*;
pub use game_manager::*;
pub use scheduler::*;

use super::entity::{WorldObject, Creature, Character};
static COMBATMANAGER: CombatManager = CombatManager::new();
pub fn combat_manager() -> &'static CombatManager{
    &COMBATMANAGER
}
///call on object spawn
pub fn on_object_spawn(object: *const ActorSpawnedEvent){
    if let Some(object) = engine_api::create_spawn(object){        
        match object {
            SpawnObject::Object(wo) =>{
                WorldObjectManager::insert(WorldObject::create(wo));
            },
            SpawnObject::Creature(creature) => {                
                CreatureManager::insert(Creature::create(creature));
            },
            SpawnObject::Character(cha) => {
                CharacterManager::insert(Character::create(cha));
            },
        }
        //check combat
        if CreatureManager::len() > 0 &&
            CharacterManager::len() > 0{
                COMBATMANAGER.start_combat(
                    CharacterManager::collect_all(), 
                    CreatureManager::collect_all()
                );
        }
    }
}
pub fn on_object_destroy(object: *const ActorDestroyEvent){
    unsafe{
        if let Some(destroy) = object.as_ref() {
            if let Some(unit_state) = destroy.unit_state.as_ref(){
                match unit_state.unit_type {
                    engine_api::EUnitType::WorldObject => {
                        WorldObjectManager::remove(unit_state.uuid);
                    },
                    engine_api::EUnitType::Pet |
                    engine_api::EUnitType::Creature => {
                        CreatureManager::remove(unit_state.uuid);
                    },
                    engine_api::EUnitType::Character => {
                        CharacterManager::remove(unit_state.uuid);
                    },
                    _ => ()
                }
            }
        }
    }
}