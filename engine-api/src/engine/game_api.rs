#![allow(unused_unsafe)]
use std::ffi::c_void;

use super::*;
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u8)]
pub enum EFixeAnimState{    
	None = 0,
	InCombat = 1,
    IsDead,
    ///set knockdown state
    KnockDown,
    ///set standup state
    StandUp,
}
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u8)]
pub enum EAnimParamType
{
	Bool = 0,
	Float,
	Integer,
	Vector,
}
#[repr(C)]
pub struct AnimStateParam {
    pub param_type: EAnimParamType,
    pub ivalue: i32,
    pub value: Vector3,
}
impl Default for AnimStateParam{
    fn default() -> Self {
        Self { param_type: EAnimParamType::Bool, ivalue: 0, value: Default::default() }
    }
}
#[repr(u8)]
#[cfg_attr(
    feature = "server_mode", 
    derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)
)]
#[cfg_attr(
    not(feature = "server_mode"), 
    derive(Debug, Clone, Copy, PartialEq, Eq)
)]
pub enum EGameNotifyEventType{
    None = 0,
    /**
    * unit attr of type(SubEvent) changed
    */
    UnitAttributeChanged,
    /**
    * skill of skill bar(subEvent) casted
    */
    SkillCasted,
    /**
    * skill of skill bar (SubEvent) change to EventValue
    */
    SkillChanged,
    /**
    * target changed to EventValue
    */
    OnTargetChanged,
    /**
    * game input, used for gcd control
    */
    OnGameInput,
}
#[repr(C)]
#[derive(Clone)]
pub struct AnimHitState {
    pub location: Vector3,
    pub direction: Vector3,
    pub source_actor: *mut AActorOpaque,
}
impl AnimHitState{
    #[inline]
    pub fn new(location: Vector3, direction: Vector3, source_actor: Option<&AActor>) -> Self{
        Self { 
            location, 
            direction, 
            source_actor: source_actor.map(|a| a.inner()).unwrap_or(std::ptr::null_mut())}
    }
}
#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum EComponentType{
    CharacterMovement,
    SkeletalMesh,
    Camera,
    Capsule,
    Rust,
}
#[repr(C)]
pub struct ComponentInfo {
    pub component_type: EComponentType,
    pub component: *mut c_void
}
pub type GamePlaySetFixAnimState = unsafe extern "C" fn(uuid: i32, fixed_state: EFixeAnimState, value: AnimStateParam);
pub type GamePlaySetDynAnimState = unsafe extern "C" fn(uuid: i32, anim_name: UName, value: AnimStateParam);
pub type GamePlayGetFName = unsafe extern "C" fn(NativeString) -> UName;
pub type GamePlayFNameToString = unsafe extern "C" fn (UName) -> *const std::os::raw::c_char;
pub type GamePlayGetMontage = unsafe extern "C" fn(UName) -> *const std::os::raw::c_void;
pub type GamePlaySetMovable = unsafe extern "C" fn(i32, bool);
pub type GamePlaySetHitState = unsafe extern "C" fn (i32, AnimHitState);
pub type GamePlaySetAnimRate = unsafe extern "C" fn(*const std::os::raw::c_void, f32, f32);
pub type GamePlayGameEventNotify = unsafe extern "C" fn(EGameNotifyEventType, i32, i64, i64);
pub type GamePlayGetComponent = unsafe extern "C" fn(*const c_void, EComponentType) -> ComponentInfo;
pub type GamePlaySetFreeze = unsafe extern "C" fn(*const c_void, freezing: bool, duration: f32, override_duration: bool);
/*
* game play callbacks
*/
#[repr(C)]
pub struct GameplayFns {
    /*
    * set fixed animation state value
    */
    pub set_fixed_anim_state: GamePlaySetFixAnimState,
    /*
    * set dynamic animation state value
    */
    pub set_dyn_anim_state: GamePlaySetDynAnimState,
    pub create_unreal_name: GamePlayGetFName,
    pub name_to_string: GamePlayFNameToString,
    pub get_montage: GamePlayGetMontage,
    set_movable: GamePlaySetMovable,
    set_hit_state: GamePlaySetHitState,
    set_anim_rate: GamePlaySetAnimRate,
    game_notify_event: GamePlayGameEventNotify,
    get_component: GamePlayGetComponent,
    set_anim_freeze: GamePlaySetFreeze,
}
/**
set target movable
 */
#[inline]
pub fn set_movable(uuid: i32, movable: bool){
    unsafe{
        (bindings().gameplay_fns.set_movable)(uuid, movable);
    }
}
/**
 * on some one hit 
 * */
#[inline]
pub fn set_hit(uuid: i32, state: AnimHitState){
    unsafe{
        (bindings().gameplay_fns.set_hit_state)(uuid, state);
    }
}
/**
 * set anim play rate
 */
#[inline]
pub fn set_anim_rate(anim_instance: &UAnimInstance, rate: f32, duration: f32){
    let anim_instance: *mut UAnimInstanceOpaque = anim_instance.inner();
    unsafe{
        (bindings().gameplay_fns.set_anim_rate)(anim_instance, rate, duration);
    }
}
#[inline]
pub fn notify_game_event(event: EGameNotifyEventType, sub_event: i32, value: i64, value_ext: i64){
    unsafe{
        (bindings().gameplay_fns.game_notify_event)(event, sub_event, value, value_ext);
    }
}
///get component of actor
#[inline]
pub fn get_component(actor: &AActor, component_type:EComponentType) -> Option<ComponentInfo>{
    let ret = unsafe{
        // unreal_api!(get_component, 
            (bindings().gameplay_fns.get_component)(actor.inner(), component_type)
        // )
    };
    if ret.component.is_null(){
        None
    }
    else{
        ret.into()
    }
}

///set anim freeze
#[inline]
pub fn set_anim_freeze(anim: &UAnimInstance, duration: f32, override_duration: bool){
    unsafe{
        // unreal_api!(set_anim_freeze, 
            (bindings().gameplay_fns.set_anim_freeze)(anim.inner(), false, duration, override_duration)
        // )
    }
}
///set anim freeze
#[inline]
pub fn set_anim_not_freeze(anim: &UAnimInstance){
    unsafe{
        // unreal_api!(set_anim_not_freeze, 
            (bindings().gameplay_fns.set_anim_freeze)(anim.inner(), false, 0.0, false)
        // )
    }
}