//! animation notify manager

use engine_api::{FUnitState, EUnitType};

use crate::game::entity::IUnit;

use super::{CreatureManager, CharacterManager};

///animation notify state event
#[derive(Debug, Clone)]
pub struct AnimNotifyState{
    pub unit: i32,
    pub duration: f32,
    pub notify_id: i32,
    pub notify_value: i32,
}
pub struct AnimNofityManager;
impl AnimNofityManager{
    ///on animation notify event
    pub fn on_event(unit: *const FUnitState, duration: f32, notify_id: i32, notify_value: i32){
        if let Some(state) = unsafe{unit.as_ref()}{
            let notify_state = AnimNotifyState{unit: state.uuid, duration, notify_id, notify_value};
            match state.unit_type{
                EUnitType::Creature => {
                    CreatureManager::get_creature(state.uuid)
                    .map(|creature| creature.mut_checked().on_anim_notify(notify_state));
                },
                EUnitType::Character => {
                    CharacterManager::get_character(state.uuid)
                    .map(|who| who.mut_checked().on_anim_notify(notify_state));
                },
                _ => (),
            }
        }
    }
}