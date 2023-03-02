use std::sync::Weak;

use shared::boxed::{MutexArc, MutableBox};
use crate::game::{CharacterManager, CreatureManager};
use super::{IUnit};

/// get self ptr
pub trait SharedSelf: Sized{
    ///get shared ptr of self
    fn get_shared_of_this(&self) -> Option<MutexArc<Self>>;
    ///get weak ptr of self
    fn get_weak_of_this(&self) -> Option<Weak<MutableBox<Self>>>{
        self.get_shared_of_this().map(|arc| arc.get_weak())
    }
}
impl<T: IUnit + 'static> SharedSelf for T{
    fn get_shared_of_this(&self) -> Option<MutexArc<Self>> {
        let uuid = self.uuid();
        match self.get_type() {
            engine_api::EUnitType::Undefined => None,
            engine_api::EUnitType::WorldObject => None,
            engine_api::EUnitType::Creature => {
                CreatureManager::get_creature(uuid).and_then(|arc| arc.convert_to() )
            },
            engine_api::EUnitType::Character => CharacterManager::get_character(uuid).and_then(|arc| arc.convert_to()),
            engine_api::EUnitType::Pet => None,
        }
    }
    fn get_weak_of_this(&self) -> Option<Weak<MutableBox<Self>>> {
        let uuid = self.uuid();
        match self.get_type() {
            engine_api::EUnitType::Undefined => None,
            engine_api::EUnitType::WorldObject => None,
            engine_api::EUnitType::Creature => {
                CreatureManager::get_creature(uuid).and_then(|arc| arc.get_weak_to() )
            },
            engine_api::EUnitType::Character => CharacterManager::get_character(uuid).and_then(|arc| arc.get_weak_to()),
            engine_api::EUnitType::Pet => None,
        }
    }
}