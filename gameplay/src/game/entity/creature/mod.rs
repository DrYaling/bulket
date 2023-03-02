//! game creature
use engine_api::IPtr;

use crate::game::{animation::{PlayAnimInfo, self}, skills::Skill};

use super::{attribute::{Attribute, EAttributeType}, unit::IUnit};
pub type Monster = Creature;
mod movement;
pub use movement::*;
pub struct Creature{
    pub(super) creature: engine_api::CreatureBase,
    attribute: Attribute,
    dead: bool,
    skills: Vec<Skill>,
    motion: CreatureMotionInfo
}
impl Creature{
    pub fn create(creature: engine_api::CreatureBase) -> Self{
        Self { 
            creature, 
            attribute: Attribute::new(), 
            dead: false,
            skills: vec![],
            motion: CreatureMotionInfo::new()
        }
    }
    //debug mode
    pub fn init_as_default(&mut self){
        self.attribute.set_max_health(200);
        self.attribute.set_health(100);
        self.attribute.set_max_mana(100);
        self.attribute.set_mana(100);        
    }
}
impl IUnit for Creature{
    fn update(&mut self, delta: f32) {
        if !self.is_alive() && !self.dead{
            self.dead = true;
            let mut mesh = self.creature.anim_instance.GetSkelMeshComponent().unwrap();
            mesh.SetSimulatePhysics(true);
            mesh.SetAllMassScale(10000.0);
            mesh.SetCollisionObjectType(engine_api::ECollisionChannel::ECC_WorldStatic);
            self.creature.GetController().unwrap().StopMovement();
            info!("oh, i'm dead");
        }
    }
    #[inline]
    fn get_attr(&self, attr: EAttributeType) -> i32 {
        self.attribute.get(attr)
    }
    #[inline]
    fn set_attr(&mut self, attr: EAttributeType, value: i32) {
        self.attribute.set(attr, value)
    }
    #[inline]
    fn get_state(&self) -> &engine_api::FUnitState {
        &self.creature.state
    }
    #[inline]
    fn play_animation(&mut self, montage: PlayAnimInfo) -> bool {
        animation::play_anim_montage(&mut self.creature.anim_instance, montage)
    }
    #[inline]
    fn anim_instance(&self) -> Option<&engine_api::UAnimInstance> {
        Some(&self.creature.anim_instance)
    }

    fn get_location(&self) -> glam::Vec3 {
        self.creature.as_actor().GetActorLocation().into()
    }
    fn get_transform(&self) -> engine_api::Transform {
        self.creature.as_actor().GetTransform()
    }
    #[inline]
    fn is_me(&self, actor: engine_api::AActor) -> bool {
        self.creature.inner() == actor.inner()
    }
}