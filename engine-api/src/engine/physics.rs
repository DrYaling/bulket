use std::ffi::c_void;
use super::*;
use crate::{AActorOpaque, Quaternion, UPrimtiveOpaque, Vector3, UName, ECollisionChannel, CollisionShape};

impl Default for CollisionShape {
    fn default() -> Self {
        Self {
            data: CollisionShapeUnion {
                sphere: CollisionSphere { radius: 0.0 },
            },
            ty: CollisionShapeType::Sphere,
        }
    }
}

#[repr(C)]
pub struct LineTraceParams {
    pub ignored_actors: *const *mut AActorOpaque,
    pub ignored_actors_len: usize,
    pub channel: ECollisionChannel,
    pub enable_debug: bool,
}

#[repr(C)]
#[derive(Debug)]
pub struct OverlapResult {
    pub actor: *mut AActorOpaque,
    pub primtive: *mut UPrimtiveOpaque,
}

impl Default for OverlapResult {
    fn default() -> Self {
        Self {
            actor: std::ptr::null_mut(),
            primtive: std::ptr::null_mut(),
        }
    }
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct HitResult {
    pub actor: *mut c_void,
    pub primtive: *mut c_void,
    pub unit_uuid: i32,
    pub unit_type: EUnitType,
    pub bone_name: UName,
    pub distance: f32,
    pub normal: Vector3,
    pub location: Vector3,
    pub impact_normal: Vector3,
    pub impact_location: Vector3,
    pub pentration_depth: f32,
    pub start_penetrating: u32
}
impl HitResult{
    pub fn new() -> Self{
        Self::default()
    }
}

impl Default for HitResult {
    fn default() -> Self {
        Self {
            actor: std::ptr::null_mut(),
            primtive: std::ptr::null_mut(),
            distance: Default::default(),
            normal: Default::default(),
            location: Default::default(),
            impact_location: Default::default(),
            impact_normal: Default::default(),
            pentration_depth: Default::default(),
            start_penetrating: Default::default(),
            bone_name: UName::none(),
            unit_uuid: 0,
            unit_type: EUnitType::Undefined,
        }
    }
}

pub type GetVelocityFn = unsafe extern "C" fn(primitive: *const UPrimtiveOpaque) -> Vector3;

pub type SetVelocityFn = unsafe extern "C" fn(primitive: *mut UPrimtiveOpaque, velocity: Vector3);

pub type IsSimulatingFn = unsafe extern "C" fn(primitive: *const UPrimtiveOpaque) -> u32;

pub type AddForceFn = unsafe extern "C" fn(actor: *mut UPrimtiveOpaque, force: Vector3);

pub type AddImpulseFn = unsafe extern "C" fn(actor: *mut UPrimtiveOpaque, force: Vector3);
pub type LineTraceFn = unsafe extern "C" fn(
    start: Vector3,
    end: Vector3,
    params: LineTraceParams,
    result: &mut HitResult,
) -> u32;
pub type GetBoundingBoxExtentFn =
    unsafe extern "C" fn(primitive: *const UPrimtiveOpaque) -> Vector3;

pub type SweepFn = unsafe extern "C" fn(
    start: Vector3,
    end: Vector3,
    rotation: Quaternion,
    params: LineTraceParams,
    collision_shape: CollisionShape,
    result: &mut HitResult,
) -> u32;

pub type OverlapMultiFn = unsafe extern "C" fn(
    collision_shape: CollisionShape,
    position: Vector3,
    rotation: Quaternion,
    params: LineTraceParams,
    max_results: usize,
    result: *mut *mut OverlapResult,
) -> u32;

pub type GetCollisionShapeFn =
    unsafe extern "C" fn(primitive: *const UPrimtiveOpaque, shape: *mut CollisionShape) -> u32;

pub type SweepMultiFn = unsafe extern "C" fn(
    start: Vector3,
    end: Vector3,
    rotation: Quaternion,
    params: LineTraceParams,
    collision_shape: CollisionShape,
    max_results: usize,
    results: *mut HitResult,
) -> u32;
pub fn line_trace(
    start: Vector3,
    end: Vector3,
    params: LineTraceParams,
    result: &mut HitResult,
) -> bool{
    unsafe{ (super::bindings().physics_fns.line_trace)(start, end, params, result) != 0}
}

#[allow(unused)]
pub fn get_bounding_box_extent(primitive: *const UPrimtiveOpaque) -> Vector3{
    todo!()
}

pub fn sweep(
    start: Vector3,
    end: Vector3,
    rotation: Quaternion,
    params: LineTraceParams,
    collision_shape: CollisionShape,
    result: &mut HitResult,
) -> u32{
    unsafe{ (super::bindings().physics_fns.sweep)(start, end, rotation, params, collision_shape, result)}
}

pub fn sweep_multi(
    start: Vector3,
    end: Vector3,
    rotation: Quaternion,
    params: LineTraceParams,
    collision_shape: CollisionShape,
    max_results: usize,
    results: *mut HitResult,
) -> u32{
    unsafe{ (super::bindings().physics_fns.sweep_multi)(start, end, rotation, params, collision_shape, max_results, results)}
}
#[allow(unused)]
pub fn overlap_multi(
    collision_shape: CollisionShape,
    position: Vector3,
    rotation: Quaternion,
    params: LineTraceParams,
    max_results: usize,
    result: *mut *mut OverlapResult,
) -> u32{
    todo!()
}

#[allow(unused)]
pub fn get_collision_shape(primitive: *const UPrimtiveOpaque, shape: *mut CollisionShape) -> u32{
    todo!()
}
#[repr(C)]
pub struct PhysicsFns {
    pub get_velocity: GetVelocityFn,
    pub set_velocity: SetVelocityFn,
    pub is_simulating: IsSimulatingFn,
    pub add_force: AddForceFn,
    pub add_impulse: AddImpulseFn,
    pub line_trace: LineTraceFn,
    pub get_bounding_box_extent: GetBoundingBoxExtentFn,
    pub sweep: SweepFn,
    pub sweep_multi: SweepMultiFn,
    pub overlap_multi: OverlapMultiFn,
    pub get_collision_shape: GetCollisionShapeFn,
}
