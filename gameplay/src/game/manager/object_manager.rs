use std::collections::HashMap;


use once_cell::sync::Lazy;
use shared::boxed::MutableBox;

use crate::game::entity::{WorldObject, IUnit};

static OBJECT_MANAGER: Lazy<MutableBox<HashMap<i32, WorldObject>>> = Lazy::new(|| MutableBox::new(HashMap::new()));
#[derive(Default)]
pub struct WorldObjectManager;
impl WorldObjectManager{
    #[inline]
    pub fn new() -> Self{
        Self::default()
    }
    #[inline]
    pub fn insert(object: WorldObject){
        OBJECT_MANAGER.lock_mut().unwrap().insert(object.get_state().uuid, object);
    }
    #[inline]
    pub fn remove(uuid: i32) -> Option<WorldObject>{
        OBJECT_MANAGER.lock_mut().unwrap().remove(&uuid)
    }
}