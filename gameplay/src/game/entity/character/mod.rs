//! game character
mod movement;
pub use movement::*;
use engine_api::{UName, EGameInputType, EUnitType, EFixeAnimState, AnimStateParam, HitResult, IPtr, UAnimInstance, UAnimMontage, CollisionShape, CollisionSphere, UPrimitiveComponent};
use glam::Vec3;
use shared::boxed::MutexArc;

use crate::game::{
    animation::{Animation, PlayAnimInfo, self}, 
    manager::{InputManager, combat_manager}, 
    anim_notify_manager::AnimNotifyState, 
    SchedulerManager,
    skills::{Skill},
};

use super::{attribute::{Attribute, EAttributeType}, unit::{IUnit, Target}, IMotion, unit_shared::SharedSelf};


pub struct Character{
    pub(super) character: engine_api::CharacterBase,
    attribute: Attribute,
    target: Target,
    attack_tick: f32,
    attack_interval: f32,
    hit_attacked: bool,
    is_attacking: bool,
    skills: Vec<Skill>,
    motion: CharacterMotionInfo,
}
impl Character{
    pub fn create(character: engine_api::CharacterBase) -> Self{
        Self { 
            character, 
            attribute: Attribute::new(), 
            target: Target::None, 
            attack_tick: 0.0, 
            attack_interval: 2.0,
            hit_attacked: false,
            is_attacking: false,
            skills: Vec::new(),
            motion: CharacterMotionInfo::new(),
        }
    }
    //debug mode
    pub fn init_as_default(&mut self){
        self.attribute.set_max_health(200);
        self.attribute.set_health(200);
        self.attribute.set_max_mana(100);
        self.attribute.set_mana(100);       
        // self.skills.push(Skill::new(1).unwrap());
        // self.skills.push(Skill::new(2).unwrap());
        // self.skills.push(Skill::new(3).unwrap());
    }
}
impl IUnit for Character{
    #[inline]
    fn anim_instance(&self) -> Option<&engine_api::UAnimInstance> {
        Some(&self.character.anim_instance)
    }
    fn update(&mut self, delta: f32) {
        if self.target.is_none(){
            //
            self.target = combat_manager().get_target(self, EUnitType::Creature);
            if let Target::Creature(creature) =&self.target{
                info!("selecte target {}", creature.get_checked().uuid());
            }
        }
        //motion check        
        if let Some(input) = InputManager::get_input(false){
            if input.input == EGameInputType::SkillBar4 && self.movable(){
                self.set_movable(false);
            }
            else if input.input == EGameInputType::SkillBar5 && !self.movable(){
                self.set_movable(true);
            }
        }
        if self.attack_tick < self.attack_interval{            
            self.attack_tick += delta;
        }
        let input = InputManager::get_input(true);
        if self.attack_tick > self.attack_interval{
            if let Some(input) = input{
                //skill 1
                let jumping = self.character.GetCharacterMovement().unwrap().IsFalling();
                if input.input == EGameInputType::SkillBar1 && !jumping{
                    self.attack_tick %= self.attack_interval;
                    self.hit_attacked = false;
                    self.is_attacking = true;
                    self.set_movable(false);
                    //"Attack", Some("BaseAtk")
                    self.play_animation(PlayAnimInfo::create().with_name_str("Attack").with_section(UName::new("BaseAtk")));
                }
            }
        }
        let is_playing  = self.character.anim_instance.IsAnyMontagePlaying();
        if !is_playing && self.is_attacking{
            self.is_attacking = false;
            self.set_movable(true);
        }
        //todo 
        if is_playing && !self.hit_attacked{
            //check trace            
            if let Target::Creature(target) = &self.target{
                let weak_target = target.get_weak();
                let mut target = target.mut_checked();
                let mut hit_result = HitResult::default();
                //trace line ignore my self
                let mut params = engine_api::LineTraceParams{
                    ignored_actors: std::ptr::null(), 
                    ignored_actors_len: 1, 
                    enable_debug: true, 
                    channel: engine_api::ECollisionChannel::ECC_Pawn
                };
                let ignore_actors = vec![self.character.inner()];
                params.ignored_actors = ignore_actors.as_ptr();
                let location = self.get_location();
                let forward: Vec3 = self.character.as_actor().GetActorForwardVector().into();
                let target_location = forward * 125.0 + location;
                let hit = engine_api::sweep(
                    location.into(), 
                    target_location.into(), 
                    self.get_rotation().into(),
                    params, 
                    CollisionShape { 
                        data: engine_api::CollisionShapeUnion { 
                            sphere: CollisionSphere{ radius: 62.5 } 
                        }, 
                        ty: engine_api::CollisionShapeType::Sphere
                    },
                    &mut hit_result
                );
                if hit != 0 && !hit_result.actor.is_null(){
                    let hit_target = engine_api::AActor::from_ptr(hit_result.actor).unwrap();                    
                    if target.is_me(hit_target){
                        self.hit_attacked = true;
                        let hp = target.get_attr(EAttributeType::Health);
                        target.set_attr(EAttributeType::Health, hp - 20);
                        if !target.is_alive(){
                            target.play_animation(PlayAnimInfo::create().with_name_str("MonsterDie"));
                        }
                        else{
                            //target.play_animation(PlayAnimInfo::create().with_name_str("MonsterHit"));
                            let force: Vec3 = hit_result.normal.into();
                            if let Some(mut comp) = UPrimitiveComponent::from_ptr(hit_result.primtive){
                                comp.AddForce((-force * 1000.0).into(), hit_result.bone_name, false);
                            }
                            // target.add_force_bone( (-force * 1000.0).into(), hit_result.bone_name, false);
                            //pause     

                            let this = self.get_weak_of_this();
                            let this_clone = this.clone();
                            let target_clone = weak_target.clone();
                            //use weak ptr instead of raw ptr
                            SchedulerManager::add(Box::new(move ||{
                                if let Some(Some(this)) = this_clone.as_ref().map(|w| w.upgrade()) {
                                    let mut this = this.lock_mut().unwrap();
                                    if let Some(mut montage) = this.character.anim_instance.GetCurrentActiveMontage(){
                                        this.character.anim_instance.Montage_SetPlayRate(&mut montage, 0.5);
                                    }
                                }
                                if let Some(monster) = target_clone.upgrade(){
                                    let mut target = monster.lock_mut().unwrap();
                                    let mut montage = target.creature.anim_instance.GetCurrentActiveMontage().unwrap();
                                    target.creature.anim_instance.Montage_SetPlayRate(&mut montage, 0.5);
                                }
                            }), 1, 0.3);
                            SchedulerManager::add(Box::new(move ||{
                                if let Some(Some(this)) = this.as_ref().map(|w| w.upgrade()) {
                                    let mut this = this.lock_mut().unwrap();
                                    if let Some(mut montage) = this.character.anim_instance.GetCurrentActiveMontage(){
                                        this.character.anim_instance.Montage_SetPlayRate(&mut montage, 1.0);
                                    }
                                }
                                if let Some(monster) = weak_target.upgrade(){
                                    let mut target = monster.lock_mut().unwrap();
                                    let mut montage = target.creature.anim_instance.GetCurrentActiveMontage().unwrap();
                                    target.creature.anim_instance.Montage_SetPlayRate(&mut montage, 1.0);
                                }
                            }), 1, 0.8);
                        }
                        info!("damage target final 10 hp");
                    }
                }
            }
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
        &self.character.state
    }
    #[inline]
    fn play_animation(&mut self, animation: PlayAnimInfo) -> bool {
        //self.character.PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName)
        animation::play_montage(&mut self.character, animation)
    }
    fn on_anim_notify(&mut self, notify: AnimNotifyState) {
        //扣血
        if let Target::Creature(target) = &self.target{
            let mut target = target.mut_checked();
            let hp = target.get_attr(EAttributeType::Health);
            target.set_attr(EAttributeType::Health, hp - 20);
            info!("damage target final 20 hp");
        }
    }

    #[inline]
    fn get_transform(&self) -> engine_api::Transform {
        self.character.as_actor().GetTransform()
    }
    #[inline]
    fn get_location(&self) -> glam::Vec3 {
        self.character.as_actor().GetActorLocation().into()
    }
    #[inline]
    fn leave_combat(&mut self) {
        animation::set_fixed_state(self, EFixeAnimState::InCombat, AnimStateParam::bool(true));
        self.target = Target::None;
    }
    #[inline]
    fn is_me(&self, actor: engine_api::AActor) -> bool {
        self.character.inner() == actor.inner()
    }
}