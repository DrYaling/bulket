use super::*;
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u8)]
pub enum EFixeAnimState{    
	None = 0,
	InCombat = 1,
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
    pub value: Vector3,
}
pub type GamePlaySetFixAnimState = unsafe extern "C" fn(uuid: i32, fixed_state: EFixeAnimState, value: AnimStateParam);
pub type GamePlaySetDynAnimState = unsafe extern "C" fn(uuid: i32, anim_name: UName, value: AnimStateParam);
pub type GamePlayGetFName = unsafe extern "C" fn(NativeString) -> UName;
pub type GamePlayFNameToString = unsafe extern "C" fn (UName) -> *const std::os::raw::c_char;
pub type GamePlayGetMontage = unsafe extern "C" fn(UName) -> *const std::os::raw::c_void;
pub type GamePlaySetMovable = unsafe extern "C" fn(i32, bool);
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
    pub set_movable: GamePlaySetMovable,
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