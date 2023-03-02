use std::sync::atomic::AtomicU32;

use engine_api::EUnitType;
use once_cell::sync::Lazy;
use shared::boxed::{MutexArc, MutableBox};

use crate::game::{battle::combat::Combat, entity::{Character, Creature, {Target, IUnit}}};

static COMBAT: Lazy<MutableBox<Combat>> = Lazy::new(|| MutableBox::new(Combat::default()));
pub struct CombatManager{
    combat_id: AtomicU32,
    ///选目标视野
    field_of_view: f32,
}
impl CombatManager{
    pub const fn new() -> Self{
        Self { 
            combat_id: AtomicU32::new(0),
            field_of_view: 20.0
        }
    }
    pub fn start_combat(
        &self, 
        characters: Vec<MutexArc<Character>>,
        monsters: Vec<MutexArc<Creature>>
    ){        
        {
            let mut combat = COMBAT.lock_mut().unwrap();
            if combat.is_valid(){
                warn!("combat started");
                combat.add_character(characters);
                combat.add_creature(monsters);
                return;
            }
        }
        let combat_id = self.combat_id.fetch_add(1, std::sync::atomic::Ordering::Relaxed);
        *COMBAT.lock_mut().unwrap() = Combat::new(combat_id, characters, monsters) ;
    }
    pub(super) fn update(&self, delta: f32){
        let mut combat = COMBAT.lock_mut().unwrap();
        if combat.is_valid(){
            combat.update(delta);
        }
    }
    pub fn get_target<TUnit: IUnit>(&self, from: &TUnit, target_type: EUnitType) -> Target{
        if !COMBAT.lock_mut().unwrap().is_valid(){
            return Target::None;
        }
        let from_location = from.get_location();
        let fov = from.field_of_view();
        match target_type {
            EUnitType::Creature => {
                let monsters = &COMBAT.lock_ref().unwrap().monsters;
                //in field of view
                for monster in monsters {
                    let location = monster.get_checked().get_location();
                    let distance = location.distance(from_location);
                    info!("distance from {:?} to {:?} is {}", from_location, location, distance);
                    if distance <= fov{
                        return Target::Creature(monster.clone());
                    }
                }
                Target::None
            },
            EUnitType::Character => {                
                let characters = &COMBAT.lock_ref().unwrap().characters;                
                //in field of view
                for character in characters {
                    if character.get_checked().get_location().distance(from_location) <= fov{
                        return Target::Character(character.clone());
                    }
                }
                Target::None
            }
            _ => Target::None
        }
    }
}