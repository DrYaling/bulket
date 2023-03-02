//! c++ wrappers

use std::ops::{Deref, DerefMut};

use crate::*;
///spawn object type
pub enum SpawnObject{
    Object(ObjectBase),
    Creature(CreatureBase),
    Character(CharacterBase),
}
///create spawn game object
pub fn create_spawn(spawn_event: *const ActorSpawnedEvent) -> Option<SpawnObject>{
    unsafe{
        let spawn_event = spawn_event.as_ref()?;
        let state = spawn_event.unit_state.as_ref()?.clone();
        let obj = match state.unit_type{
            EUnitType::WorldObject => SpawnObject::Object(ObjectBase{ 
                entity: AActor::from_ptr(spawn_event.actor)?, 
                anim_instance: UAnimInstance::from_ptr(state.animation),
                state 
            }),
            EUnitType::Creature => SpawnObject::Creature(CreatureBase{ 
                entity: APawn::from_ptr(spawn_event.actor)?, 
                anim_instance: UAnimInstance::from_ptr(state.animation).unwrap(),
                state 
            }),
            EUnitType::Character |
            EUnitType::Pet => SpawnObject::Character(CharacterBase{ 
                entity: ACharacter::from_ptr(spawn_event.actor)?, 
                anim_instance: UAnimInstance::from_ptr(state.animation).unwrap(),
                state
            }),
            EUnitType::Undefined => return None,
        };
        Some(obj)
    }
}
/// c++ AActor entity
pub struct ObjectBase{
    /// c++ ARustActor
    entity: AActor,    
    pub state: FUnitState,
    pub anim_instance: Option<UAnimInstance>,
}
impl ObjectBase{
}
impl Deref for ObjectBase{
    type Target = AActor;
    #[inline]
    fn deref(&self) -> &Self::Target {
        &self.entity
    }
}
impl DerefMut for ObjectBase{
    #[inline]
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.entity
    }
}
///c++ APawn entity
pub struct CreatureBase{
    /// c++ ARustPawn
    entity: APawn,  
    pub state: FUnitState,
    pub anim_instance: UAnimInstance,
}

impl Deref for CreatureBase{
    type Target = APawn;
    #[inline]
    fn deref(&self) -> &Self::Target {
        &self.entity
    }
}
impl DerefMut for CreatureBase{
    #[inline]
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.entity
    }
}
//c++ character entity
pub struct CharacterBase{
    /// c++ ARustCharacter
    entity: ACharacter,  
    pub anim_instance: UAnimInstance,
    pub state: FUnitState,
}
impl Deref for CharacterBase{
    type Target = ACharacter;
    #[inline]
    fn deref(&self) -> &Self::Target {
        &self.entity
    }
}
impl DerefMut for CharacterBase{
    #[inline]
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.entity
    }
}