use std::collections::HashMap;

use once_cell::sync::Lazy;
use shared::boxed::{MutexArc, MutableBox};

use crate::game::entity::{Creature, IUnit};

static CREATURE_MANAGER: Lazy<MutableBox<HashMap<i32, MutexArc<Creature>>>> = Lazy::new(|| MutableBox::new(HashMap::new()));
#[derive(Default)]
pub struct CreatureManager;
impl CreatureManager{
    #[inline]
    pub fn new() -> Self{
        Self::default()
    }
    #[inline]
    pub fn insert(mut creature: Creature){
        creature.init_as_default();
        info!("new creature {}", creature.uuid());
        CREATURE_MANAGER.lock_mut().unwrap().insert(creature.get_state().uuid, MutexArc::new(creature));
    }
    #[inline]
    pub fn remove(uuid: i32) -> Option<MutexArc<Creature>>{
        CREATURE_MANAGER.lock_mut().unwrap().remove(&uuid)
    }
    #[inline]
    pub fn get_creature(uuid: i32) -> Option<MutexArc<Creature>>{
        CREATURE_MANAGER.lock_ref().unwrap().get(&uuid).cloned()
    }
    #[inline]
    pub fn len() -> usize{
        CREATURE_MANAGER.lock_ref().unwrap().len()
    }
    #[inline]
    pub fn collect_all() -> Vec<MutexArc<Creature>>{
        CREATURE_MANAGER.lock_ref().unwrap().values().cloned().collect()
    }
    #[inline]
    pub(super) fn update(delta: f32){
        CREATURE_MANAGER.lock_ref().unwrap()
        .iter().for_each(|(_, who)|{
            who.mut_checked().update(delta)
        });
    }
}