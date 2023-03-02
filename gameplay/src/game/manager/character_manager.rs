//! thread safe unit manager manage 

use std::collections::{HashMap};
use once_cell::sync::Lazy;
use shared::boxed::{MutexArc, MutableBox, LockableBox};

use crate::game::entity::{Character, IUnit};
///这种方式保存角色,在进行计算时,会比较慢,后面再考虑怎么处理
static CHARACTER_MANAGER: Lazy<MutableBox<HashMap<i32, MutexArc<Character>>>> = Lazy::new(|| MutableBox::new(HashMap::new()));
///unit manager stores all game entities
#[derive(Default)]
pub struct CharacterManager;
impl CharacterManager{
    #[inline]
    pub fn new() -> Self{
        Self::default()
    }
    #[inline]
    pub fn insert(mut character: Character){
        character.init_as_default();
        info!("new character {}", character.uuid());
        CHARACTER_MANAGER.lock_mut().unwrap().insert(character.get_state().uuid, MutexArc::new(character));
    }
    #[inline]
    pub fn remove(uuid: i32) -> Option<MutexArc<Character>>{
        CHARACTER_MANAGER.lock_mut().unwrap().remove(&uuid)
    }
    #[inline]
    pub fn get_character(uuid: i32) -> Option<MutexArc<Character>>{
        CHARACTER_MANAGER.lock_ref().unwrap().get(&uuid).cloned()
    }
    #[inline]
    pub fn len() -> usize{
        CHARACTER_MANAGER.lock_ref().unwrap().len()
    }
    #[inline]
    pub fn collect_all() -> Vec<MutexArc<Character>>{
        CHARACTER_MANAGER.lock_ref().unwrap().values().cloned().collect()
    }
    #[inline]
    pub(super) fn update(delta: f32){
        CHARACTER_MANAGER.lock_ref().unwrap()
        .iter().for_each(|(_, who)|{
            who.mut_checked().update(delta)
        });
    }
}