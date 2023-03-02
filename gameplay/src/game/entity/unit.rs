use engine_api::{EUnitType, FUnitState, AnimStateParam, Transform, UAnimInstance, UName, EFixeAnimState, AActor};
use glam::{Vec3, Quat};
use shared::boxed::MutexArc;

use crate::game::{
    animation::{PlayAnimInfo, self}, 
    manager::anim_notify_manager::AnimNotifyState
};

use super::{attribute::EAttributeType, character::Character, creature::Creature, object::WorldObject};
/**
 * unit api
 */
pub trait IUnit: Sized{
    #[inline]
    fn anim_instance(&self) -> Option<&UAnimInstance>{ None }
    #[allow(unused)]
    #[inline]
    fn update(&mut self, delta: f32){}
    #[allow(unused)]
    #[inline]
    fn is_alive(&self) -> bool{
        self.get_attr(EAttributeType::Health) > 0
    }
    ///get unit uuid
    #[inline]
    fn uuid(&self) -> i32{
        self.get_state().uuid
    }
    #[allow(unused)]
    #[inline]
    fn get_attr(&self, attr: EAttributeType) -> i32{ 0 }
    #[allow(unused)]
    #[inline]
    fn set_attr(&mut self, attr: EAttributeType, value: i32){
        
    }
    #[inline]
    fn get_type(&self) -> EUnitType{
        self.get_state().unit_type
    }
    fn get_state(&self) -> &FUnitState;
    #[allow(unused)]
    #[inline]
    fn play_animation(&mut self, animation: PlayAnimInfo) -> bool{ false }
    #[allow(unused)]
    #[inline]
    fn set_anim_state(&self, anim_state: UName, state: engine_api::AnimStateParam) -> bool {
        // self.character.set_anim_state(anim_state, state)
        animation::set_anim_state(self, anim_state, state)
    }
    #[inline]
    fn set_fixed_state(&self, fixed_state: EFixeAnimState, state: engine_api::AnimStateParam) -> bool{
        animation::set_fixed_state(self, fixed_state, state)
    }
    #[inline]
    fn on_anim_notify(&mut self, _notify: AnimNotifyState){

    }    
    fn get_transform(&self) -> Transform;
    #[inline]
    fn get_location(&self) -> Vec3{
        self.get_transform().location.into()
    }
    #[inline]
    fn get_rotation(&self) -> Quat{
        self.get_transform().rotation.into()
    }
    #[inline]
    fn get_scale(&self) -> f32 { 1.0 }
    #[inline]
    fn enter_combat(&mut self){
        animation::set_fixed_state(self, EFixeAnimState::InCombat, AnimStateParam::bool(true));
    }
    #[inline]
    fn leave_combat(&mut self){
        animation::set_fixed_state(self, EFixeAnimState::InCombat, AnimStateParam::bool(false));
    }
    //distance in miters this unit can see target
    #[inline]
    fn field_of_view(&self) -> f32 { 3000.0 }
    ///check if actor is targeted to me?
    fn is_me(&self, actor: AActor) -> bool;
    
}
/**
 * unit motion api
 */
pub trait IMotion{
    /**
     * set target movable
     */
    #[inline]
    fn set_movable(&mut self, _movable: bool){}
    /**
     * can unit move?
     */
    #[inline]
    fn movable(&self) -> bool { false }
    /**
     * add force to target
     */
    #[allow(unused)]
    #[inline]
    fn add_force(&mut self, force: Vec3){}
    /**
     * add force to target bone
     */
    #[allow(unused)]
    #[inline]
    fn add_force_bone(&mut self, force: Vec3, bone: UName, use_acces: bool){}
}
#[derive(Clone)]
#[allow(unused)]
pub enum Target{
    Character(MutexArc<Character>),
    Creature(MutexArc<Creature>),
    WorldObject(MutexArc<WorldObject>),
    None,
}
#[allow(unused)]
impl Target{
    #[inline]
    pub fn is_none(&self) -> bool { matches!(self, Self::None)}
    #[inline]
    pub fn is_character(&self) -> bool { matches!(self, Self::Character(_))}
    #[inline]
    pub fn is_creature(&self) -> bool { matches!(self, Self::Creature(_))}
    #[inline]
    pub fn is_object(&self) -> bool { matches!(self, Self::WorldObject(_))}
}