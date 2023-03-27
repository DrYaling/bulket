use glam::Vec3;

use crate::{*};
use std::{ffi::c_void, os::raw::c_char};

#[repr(u8)]
#[derive(Debug)]
pub enum ResultCode {
    Success = 0,
    Panic = 1,
}
#[repr(u32)]
#[derive(Copy, Clone)]
pub enum UObjectType {
    UClass,
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
pub enum EGameInputType {
    None = 0,
    ControlA,
    ControlB,
    ControlC,
    ControlD,
    SkillBar1,
    SkillBar2,
    SkillBar3,
    SkillBar4,
    SkillBar5,
}
impl EGameInputType{
    pub fn new(key: u8) -> Self{
        use EGameInputType::*;
        match key {
            1 => ControlA,
            2 => ControlB,
            3 => ControlC,
            4 => ControlD,
            5 => SkillBar1,
            6 => SkillBar2,
            7 => SkillBar3,
            8 => SkillBar4,
            9 => SkillBar5,
            _ => EGameInputType::None
        }
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
pub enum ETriggerEvent
{
	// No significant trigger state changes occurred and there are no active device inputs
	None = 0,

	// Triggering occurred after one or more processing ticks
	Triggered,				// ETriggerState (None -> Triggered, Ongoing -> Triggered, Triggered -> Triggered)
	
	// An event has occurred that has begun Trigger evaluation. Note: Triggered may also occur this frame.
	Started,				// ETriggerState (None -> Ongoing, None -> Triggered)

	// Triggering is still being processed
	Ongoing,				// ETriggerState (Ongoing -> Ongoing)

	// Triggering has been canceled
	Canceled,				// ETriggerState (Ongoing -> None)

	// The trigger state has transitioned from Triggered to None this frame, i.e. Triggering has finished.
	// NOTE: Using this event restricts you to one set of triggers for Started/Completed events. You may prefer two actions, each with its own trigger rules.
	// TODO: Completed will not fire if any trigger reports Ongoing on the same frame, but both should fire. e.g. Tick 2 of Hold (= Ongoing) + Pressed (= None) combo will raise Ongoing event only.
	Completed,				// ETriggerState (Triggered -> None)
}

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum REndPlayReason
{
    /** When the Actor or Component is explicitly destroyed. */
    Destroyed,
    /** When the world is being unloaded for a level transition. */
    LevelTransition,
    /** When the world is being unloaded because PIE is ending. */
    EndPlayInEditor,
    /** When the level it is a member of is streamed out. */
    RemovedFromWorld,
    /** When the application is being exited. */
    Quit,
}
///unit type of game
#[repr(u8)]
#[cfg_attr(
    feature = "server_mode", 
    derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)
)]
#[cfg_attr(
    not(feature = "server_mode"), 
    derive(Debug, Clone, Copy, PartialEq, Eq)
)]
pub enum EUnitType
{
	/**
	 *  not initialized unit
	 */
	Undefined,
	/**
	 *  static world object
	 */
	WorldObject,
	/**
	 *  creature with ai controller
	 */
	Creature,
	/**
	 *  character controlled by player
	 */
	Character,
	/**
	 *  pet controlled by ai & player
	 */
	Pet,
}
///unit spawn state
#[repr(C)]
#[derive(Clone)]
pub struct FUnitState
{
	/**
	 *  spawn location of X
	 */
	pub spawn_x: f32,
	/**
	 *  spawn location of Y
	 */
	pub spawn_y: f32,
	/**
	 *  spawn location of Z
	 */
	pub spawn_z: f32,
	/**
	 *  owner of this state
	 */
	pub(crate) owner: *mut AActorOpaque,
	/**
	 *  spawn scale
	 */
	pub spawn_scale: f32,

	/**
	 *  Unit template Id
	 */
	pub template_id: i32,

	/**
	 *  Unit Id
	 * based on UnitType
	 */
	pub unit_id: i32,
	/**
	 *  level Id if it's creature or worldObject
	 */
	pub level_id: i32,
	/**
	 *  unit type of this unit
	 */
	pub unit_type: EUnitType,
	/**
	 * object uuid of game
	 */
	pub uuid: i32,
	/*
	* animation of this object
	*/
	pub(crate) animation: *mut c_void,

}
#[repr(C)]
pub struct Utf8Str {
    pub ptr: *const c_char,
    pub len: usize,
}

impl<'a> From<&'a str> for Utf8Str {
    fn from(s: &'a str) -> Self {
        Self {
            ptr: s.as_ptr() as *const c_char,
            len: s.len(),
        }
    }
}



#[repr(C)]
#[derive(Default, Debug, Copy, Clone)]
pub struct Movement {
    pub velocity: Vector3,
    pub is_falling: u32,
}


// TODO: Is there a more typesafe way of defining an opaque type that
// is c ffi safe in Rust without nightly?
pub type UPrimtiveOpaque = c_void;
pub type UCapsuleOpaque = c_void;
pub type UClassOpague = c_void;
pub type USoundBaseOpague = c_void;

pub type LogFn = extern "C" fn(*const c_char, i32, i32);
pub type IterateActorsFn = unsafe extern "C" fn(array: *mut *mut AActorOpaque, len: *mut u64);
pub type GetActionStateFn =
    unsafe extern "C" fn(name: *const c_char, len: usize, state: ActionState, out: *mut u32);
pub type GetAxisValueFn = unsafe extern "C" fn(name: *const c_char, len: usize, value: &mut f32);
pub type SpawnActorFn = unsafe extern "C" fn(
    actor_class: ActorClass,
    position: Vector3,
    rotation: Quaternion,
    scale: Vector3,
) -> *mut AActorOpaque;
pub type SpawnActorBPFn = unsafe extern "C" fn(
    bp_name: NativeString,
    transform: Transform,
    actor_type: ActorClass,
) -> *mut AActorOpaque;
pub type GetMouseDeltaFn = unsafe extern "C" fn(x: &mut f32, y: &mut f32);
pub type VisualLogSegmentFn =
    unsafe extern "C" fn(owner: *const AActorOpaque, start: Vector3, end: Vector3, color: Color);
pub type VisualLogCapsuleFn = unsafe extern "C" fn(
    category: Utf8Str,
    owner: *const AActorOpaque,
    position: Vector3,
    rotation: Quaternion,
    half_height: f32,
    radius: f32,
    color: Color,
);

pub type VisualLogLocationFn = unsafe extern "C" fn(
    category: Utf8Str,
    owner: *const AActorOpaque,
    position: Vector3,
    radius: f32,
    color: Color,
);

pub type SetSpatialDataFn = extern "C" fn(
    actor: *mut AActorOpaque,
    position: Vector3,
    rotation: Quaternion,
    scale: Vector3,
);

pub type GetSpatialDataFn = extern "C" fn(
    actor: *const AActorOpaque,
    position: &mut Vector3,
    rotation: &mut Quaternion,
    scale: &mut Vector3,
);

pub type GetActorComponentsFn =
    unsafe extern "C" fn(actor: *const AActorOpaque, data: *mut ActorComponentPtr, len: &mut usize);

pub type GetRootComponentFn =
    unsafe extern "C" fn(actor: *const AActorOpaque, data: *mut ActorComponentPtr);

pub type GetRegisteredClassesFn =
    unsafe extern "C" fn(classes: *mut *mut UClassOpague, len: *mut usize);

pub type GetClassFn = unsafe extern "C" fn(actor: *const AActorOpaque) -> *mut UClassOpague;

pub type IsMoveableFn = unsafe extern "C" fn(actor: *const AActorOpaque) -> u32;

pub type GetActorNameFn = unsafe extern "C" fn(actor: *const AActorOpaque, data: *mut RustAlloc);

pub type SetOwnerFn =
    unsafe extern "C" fn(actor: *mut AActorOpaque, new_owner: *const AActorOpaque);

pub type RegisterActorOnOverlapFn = unsafe extern "C" fn(actor: *mut AActorOpaque);
pub type RegisterActorOnHitFn = unsafe extern "C" fn(actor: *mut AActorOpaque);

pub type SetViewTargetFn = unsafe extern "C" fn(actor: *const AActorOpaque);

pub type DestroyActorFn = unsafe extern "C" fn(actor: *const AActorOpaque);

type SetActorLocationOrRotationFn = unsafe extern "C" fn(
    actor: *mut AActorOpaque, 
    vector: Vector3,
    set_location: bool,
    hit: *mut HitResult,
    teleport: ETeleportType,
) -> bool;
#[repr(C)]
pub struct ActorFns {
    pub get_spatial_data: GetSpatialDataFn,
    pub set_spatial_data: SetSpatialDataFn,
    pub get_actor_components: GetActorComponentsFn,
    pub register_actor_on_overlap: RegisterActorOnOverlapFn,
    pub register_actor_on_hit: RegisterActorOnHitFn,
    pub get_root_component: GetRootComponentFn,
    pub get_registered_classes: GetRegisteredClassesFn,
    pub get_class: GetClassFn,
    pub set_view_target: SetViewTargetFn,
    pub destroy_actor: DestroyActorFn,
    set_actor_location_or_rotation: SetActorLocationOrRotationFn
}
#[repr(C)]
pub struct UnrealBindings {
    pub actor_fns: ActorFns,
    pub physics_fns: PhysicsFns,
    pub log: LogFn,
    pub iterate_actors: IterateActorsFn,
    pub get_action_state: GetActionStateFn,
    pub get_axis_value: GetAxisValueFn,
    pub spawn_actor: SpawnActorFn,
    pub spawn_bp_actor: SpawnActorBPFn,
    pub get_mouse_delta: GetMouseDeltaFn,
    pub visual_log_segment: VisualLogSegmentFn,
    pub visual_log_capsule: VisualLogCapsuleFn,
    pub visual_log_location: VisualLogLocationFn,
    pub editor_component_fns: EditorComponentFns,
    pub gameplay_fns: GameplayFns,
    // pub sound_fns: SoundFns,
}
unsafe impl Sync for UnrealBindings {}
unsafe impl Send for UnrealBindings {}

#[repr(u8)]
#[derive(Debug)]
pub enum ActionState {
    Pressed = 0,
    Released = 1,
    Held = 2,
}
#[repr(u32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ActorClass {
    RustActor = 0,
    CameraActor = 1,
    RustCharacter = 2,
    RustPawn = 3,
    EngineActor = 4,
}
#[repr(u32)]
#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub enum ActorComponentType {
    Primitive,
}

#[repr(u8)]
#[derive(Debug)]
pub enum Mobility {
    Static = 0,
    Stationary = 1,
    Moveable = 2,
}

#[repr(C)]
#[derive(Debug)]
pub struct ActorComponentPtr {
    pub ty: ActorComponentType,
    pub ptr: *mut c_void,
}

impl Default for ActorComponentPtr {
    fn default() -> Self {
        Self {
            ty: ActorComponentType::Primitive,
            ptr: std::ptr::null_mut(),
        }
    }
}


pub type EntryUnrealBindingsFn =
unsafe extern "C" fn(bindings: UnrealBindings, rust_bindings: *mut RustBindings) -> u32;
pub type BeginPlayFn = unsafe extern "C" fn() -> ResultCode;
pub type TickFn = unsafe extern "C" fn(dt: std::ffi::c_float) -> ResultCode;
pub type VoidFn = unsafe extern "C" fn() -> ResultCode;
pub type BoolFn = unsafe extern "C" fn(bool) -> ResultCode;
pub type EndPlayFn = unsafe extern "C" fn(REndPlayReason) -> ResultCode;
pub type LogLevelFn = unsafe extern "C" fn(ll: i32) -> ResultCode;
pub type OnGameInputFn = unsafe extern "C" fn(input: EGameInputType, ETriggerEvent, f32) -> ResultCode;
pub type RetrieveUuids = unsafe extern "C" fn(ptr: *mut Uuid, len: *mut usize);
pub type GamePlaySetUnitSkills = unsafe extern "C" fn(uuid: i32, skills: *mut SkillWithKeyBinding, skill_len: u32) -> ResultCode;
pub type GetVelocityRustFn = unsafe extern "C" fn(actor: *const AActorOpaque, velocity: &mut Vector3);
#[repr(C)]
#[cfg_attr(
    feature = "server_mode", 
    derive(Debug, Clone, Copy, Serialize, Deserialize)
)]
#[cfg_attr(
    not(feature = "server_mode"), 
    derive(Debug, Clone, Copy)
)]
pub struct SkillWithKeyBinding{
    pub skill_id: u32,
    pub key: EGameInputType,
}
#[repr(u32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum EventType {
    ActorSpawned = 0,
    ActorBeginOverlap = 1,
    ActorEndOverlap = 2,
    ActorOnHit = 3,
    ActorDestroy = 4,
}

#[repr(C)]
pub struct ActorSpawnedEvent {
    pub actor: *mut AActorOpaque,
    pub unit_state: *const FUnitState,
}

#[repr(C)]
pub struct ActorBeginOverlap {
    pub overlapped_actor: *mut AActorOpaque,
    pub other: *mut AActorOpaque,
}
#[repr(C)]
pub struct ActorEndOverlap {
    pub overlapped_actor: *mut AActorOpaque,
    pub other: *mut AActorOpaque,
}

#[repr(C)]
pub struct ActorHitEvent {
    pub self_actor: *mut AActorOpaque,
    pub other: *mut AActorOpaque,
    pub normal_impulse: Vector3,
}

#[repr(C)]
pub struct ActorDestroyEvent {
    pub actor: *mut AActorOpaque,
    pub unit_state: *const FUnitState,
}

#[repr(C)]
pub struct RustBindings {
    // pub retrieve_uuids: RetrieveUuids,
    pub log_level: LogLevelFn,
    pub init: VoidFn,
    pub destroy: VoidFn,
    pub tick: TickFn,
    pub begin_play: BeginPlayFn,
    pub unreal_event: UnrealEventFn,
    pub end_play: EndPlayFn,
    pub on_pause: BoolFn,
    pub on_game_input: OnGameInputFn,
    pub anim_fns: AnimationFns,
    pub set_unit_skills: GamePlaySetUnitSkills,
    // pub reflection_fns: ReflectionFns,
    // pub allocate_fns: AllocateFns,
}

pub type UnrealEventFn = unsafe extern "C" fn(ty: *const EventType, data: *const c_void);

#[repr(u32)]
pub enum ReflectionType {
    Float,
    Vector3,
    Bool,
    Quaternion,
    UClass,
    USound,
    Composite,
}

pub type NumberOfFieldsFn = unsafe extern "C" fn(uuid: Uuid, out: *mut u32) -> u32;
pub type GetTypeNameFn = unsafe extern "C" fn(uuid: Uuid, name: *mut Utf8Str) -> u32;
pub type GetFieldNameFn =
    unsafe extern "C" fn(uuid: Uuid, field_idx: u32, name: *mut Utf8Str) -> u32;
pub type GetFieldTypeFn =
    unsafe extern "C" fn(uuid: Uuid, field_idx: u32, ty: *mut ReflectionType) -> u32;

pub type GetFieldFloatValueFn =
    unsafe extern "C" fn(uuid: Uuid, entity: Entity, field_idx: u32, out: *mut f32) -> u32;
pub type GetFieldVector3ValueFn =
    unsafe extern "C" fn(uuid: Uuid, entity: Entity, field_idx: u32, out: *mut Vector3) -> u32;
pub type GetFieldBoolValueFn =
    unsafe extern "C" fn(uuid: Uuid, entity: Entity, field_idx: u32, out: *mut u32) -> u32;
pub type GetFieldQuatValueFn =
    unsafe extern "C" fn(uuid: Uuid, entity: Entity, field_idx: u32, out: *mut Quaternion) -> u32;
pub type HasComponentFn = unsafe extern "C" fn(entity: Entity, uuid: Uuid) -> u32;
pub type IsEditorComponentFn = unsafe extern "C" fn(uuid: Uuid) -> u32;

#[repr(C)]
pub struct ReflectionFns {
    pub is_editor_component: IsEditorComponentFn,
    pub number_of_fields: NumberOfFieldsFn,
    pub has_component: HasComponentFn,
    pub get_type_name: GetTypeNameFn,
    pub get_field_type: GetFieldTypeFn,
    pub get_field_name: GetFieldNameFn,
    pub get_field_vector3_value: GetFieldVector3ValueFn,
    pub get_field_bool_value: GetFieldBoolValueFn,
    pub get_field_float_value: GetFieldFloatValueFn,
    pub get_field_quat_value: GetFieldQuatValueFn,
}

#[repr(C)]
pub struct RustAlloc {
    pub ptr: *mut u8,
    pub size: usize,
    pub align: usize,
}

impl RustAlloc {
    pub fn empty() -> Self {
        Self {
            ptr: std::ptr::null_mut(),
            size: 0,
            align: 0,
        }
    }
    /// # Safety
    /// Must have a valid allocation from within unreal c++
    /// Only free if the ptr is not already in use
    /// Ptr must be valid, and allocated from the same allocator
    pub unsafe fn free(self) {
        if self.size == 0 || self.ptr.is_null() {
            return;
        }
        std::alloc::dealloc(
            self.ptr,
            std::alloc::Layout::from_size_align(self.size, self.align).unwrap(),
        );
    }
}

pub type AllocateFn = unsafe extern "C" fn(size: usize, align: usize, ptr: *mut RustAlloc) -> u32;

#[repr(C)]
pub struct AllocateFns {
    pub allocate: AllocateFn,
}

extern "C" {
    pub fn GetEditorComponentUuids(
        actor: *const AActorOpaque,
        data: *mut Uuid,
        len: *mut usize,
    ) -> u32;

    pub fn GetEditorComponentVector(
        actor: *const AActorOpaque,
        uuid: Uuid,
        field: Utf8Str,
        out: *mut Vector3,
    ) -> u32;
    pub fn GetEditorComponentFloat(
        actor: *const AActorOpaque,
        uuid: Uuid,
        field: Utf8Str,
        out: *mut f32,
    ) -> u32;
    pub fn GetEditorComponentBool(
        actor: *const AActorOpaque,
        uuid: Uuid,
        field: Utf8Str,
        out: *mut u32,
    ) -> u32;
    pub fn GetEditorComponentQuat(
        actor: *const AActorOpaque,
        uuid: Uuid,
        field: Utf8Str,
        out: *mut Quaternion,
    ) -> u32;
    pub fn GetEditorComponentUObject(
        actor: *const AActorOpaque,
        uuid: Uuid,
        field: Utf8Str,
        ty: UObjectType,
        out: *mut *mut UObjectOpaque,
    ) -> u32;
}

pub type GetEditorComponentUuidsFn =
    unsafe extern "C" fn(actor: *const AActorOpaque, data: *mut Uuid, len: *mut usize) -> u32;

pub type GetEditorComponentQuatFn = unsafe extern "C" fn(
    actor: *const AActorOpaque,
    uuid: Uuid,
    field: Utf8Str,
    out: *mut Quaternion,
) -> u32;

pub type GetEditorComponentVectorFn = unsafe extern "C" fn(
    actor: *const AActorOpaque,
    uuid: Uuid,
    field: Utf8Str,
    out: *mut Vector3,
) -> u32;

pub type GetEditorComponentFloatFn = unsafe extern "C" fn(
    actor: *const AActorOpaque,
    uuid: Uuid,
    field: Utf8Str,
    out: *mut f32,
) -> u32;

pub type GetEditorComponentBoolFn = unsafe extern "C" fn(
    actor: *const AActorOpaque,
    uuid: Uuid,
    field: Utf8Str,
    out: *mut u32,
) -> u32;
pub type GetEditorComponentUObjectFn = unsafe extern "C" fn(
    actor: *const AActorOpaque,
    uuid: Uuid,
    field: Utf8Str,
    ty: UObjectType,
    out: *mut *mut UObjectOpaque,
) -> u32;

#[repr(C)]
pub struct EditorComponentFns {
    pub get_editor_components: GetEditorComponentUuidsFn,
    pub get_editor_component_quat: GetEditorComponentQuatFn,
    pub get_editor_component_vector: GetEditorComponentVectorFn,
    pub get_editor_component_bool: GetEditorComponentBoolFn,
    pub get_editor_component_float: GetEditorComponentFloatFn,
    pub get_editor_component_uobject: GetEditorComponentUObjectFn,
}

pub type AnimNotifyFn = unsafe extern "C" fn(
    unit: *const FUnitState,
    duration: f32,
    notify_id: i32,
    notify_value: i32,
) -> ResultCode;
#[repr(C)]
pub struct AnimationFns{
    pub notify_fn: AnimNotifyFn,
}
#[inline]
pub fn set_actor_location(actor: &AActor, location: Vec3, teleport: ETeleportType) -> bool{
    unsafe{
        (bindings().actor_fns.set_actor_location_or_rotation)(actor.inner(), location.into(), true, std::ptr::null_mut(), teleport)
    }
}
///set actor location,check collision
#[inline]
pub fn set_actor_location_checked(actor: &AActor, location: Vec3, hit_result: &mut HitResult, teleport: ETeleportType) -> bool{
    unsafe{
        (bindings().actor_fns.set_actor_location_or_rotation)(actor.inner(), location.into(), true, hit_result as _, teleport)
    }
}
#[inline]
pub fn set_actor_rotation(actor: &AActor, rotation: Vec3) -> bool{
    unsafe{
        (bindings().actor_fns.set_actor_location_or_rotation)(actor.inner(), rotation.into(), false, std::ptr::null_mut(), ETeleportType::None)
    }
}