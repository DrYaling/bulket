//! engine registered callbacks
//! 
use engine_api::*;
use std::{ffi::c_void};

use crate::{game::{InputManager, anim_notify_manager::AnimNofityManager, GameManager}};
pub unsafe extern "C" fn on_game_input(input: EGameInputType, trigger_event: ETriggerEvent, elapsed: f32) -> ResultCode{
    if input != EGameInputType::None{
        return InputManager::on_game_input(input, trigger_event, elapsed);
    }
    ResultCode::Success
}
pub unsafe extern "C" fn unreal_event(ty: *const EventType, data: *const c_void) {
    // debug!("unreal_event");
    if data.is_null(){
        info!("data is null");
        return;
    }
    match std::panic::catch_unwind(||{
        match *ty {
            EventType::ActorSpawned => {
                let ref_ptr = data as *const ActorSpawnedEvent;
                let actor_spawned_event = ref_ptr.as_ref().unwrap();
                let actor = AActor::from_ptr(actor_spawned_event.actor).unwrap();
                // let name = actor.GetActorNameOrLabel();
                // let world = actor.as_uobject().GetWorld().unwrap();
                if !actor_spawned_event.unit_state.is_null() && (*(actor_spawned_event.unit_state)).unit_type != EUnitType::Undefined{
                    //  (ActorClass::RustActor, )
                    register_on_hit(&actor);
                    register_overlap(&actor);
                    crate::game::on_object_spawn(ref_ptr);
                //     info!("on event {:?} for actor {} at {:?}: {:?} {:p}", 
                //     *ty, name, actor.GetActorLocation(), actor.GetOwner().map(|owner| owner.GetActorNameOrLabel()), 
                //     world.inner()
                // );
                }
            }
            EventType::ActorBeginOverlap => {
                let overlap = data as *const ActorBeginOverlap;
                let actor1 = AActor::from_ptr(overlap.as_ref().unwrap().overlapped_actor).unwrap();
                let actor2 = AActor::from_ptr(overlap.as_ref().unwrap().other).unwrap();
                info!("on event {:?} for actor {} & {}", *ty, actor1.GetActorNameOrLabel(), actor2.GetActorNameOrLabel());
            }
            EventType::ActorEndOverlap => {
                let overlap = data as *const ActorEndOverlap;
                let actor1 = AActor::from_ptr(overlap.as_ref().unwrap().overlapped_actor).unwrap();
                let actor2 = AActor::from_ptr(overlap.as_ref().unwrap().other).unwrap();
                info!("on event {:?} for actor {} & {}", *ty, actor1.GetActorNameOrLabel(), actor2.GetActorNameOrLabel());
            }
            EventType::ActorOnHit => {
                // let hit = data as *const ActorHitEvent;
                // let actor1 = AActor::from_ptr(hit.as_ref().unwrap().self_actor).unwrap();
                // let actor2 = AActor::from_ptr(hit.as_ref().unwrap().other).unwrap();
                // if actor1.GetActorNameOrLabel().to_lowercase().contains("player"){
                //     let transform = Transform{
                //         rotation: actor1.GetActorQuat(),
                //         location: actor1.GetActorLocation() + Vector3::new(30f32, 20f32, 0.0),
                //         scale: actor1.GetActorScale3D(),
                //     };
                //     info!("spawn character Rish at {:?}", actor1.GetActorLocation());
                //     if let Some(spawned) = spawn_actor("Risl", transform, ActorClass::RustCharacter){
                //         info!("spawn actor is {}", spawned.GetActorNameOrLabel());
                //     }
                // }
                // info!("on event {:?} for actor {} & {} imp {:?}", *ty, actor1.GetActorNameOrLabel(), actor2.GetActorNameOrLabel(), hit.as_ref().unwrap().normal_impulse);
            }
            EventType::ActorDestroy => {
                let destroy = data as *const ActorDestroyEvent;
                crate::game::on_object_destroy(destroy);
                let actor = AActor::from_ptr(destroy.as_ref().unwrap().actor).unwrap();
                info!("on event {:?} for actor {}", *ty, actor.GetActorNameOrLabel());
            }
        }
    }){
        Ok(_) => (),
        Err(e) => error!("fail {:?}", e) 
    }
}
pub extern "C" fn tick(dt: f32) -> ResultCode {    
    let r = std::panic::catch_unwind(||{
        GameManager::update(dt);
    });
    match r {
        Ok(_) => ResultCode::Success,
        Err(e) => {
            error!("tick panic {:?}", e);
            ResultCode::Panic
        },
    }
}

pub extern "C" fn init_native() -> ResultCode {
    debug!("init native");
    ResultCode::Success
}
pub extern "C" fn set_log_level(log_level: i32) -> ResultCode {
    debug!("init set_log_level {}", log_level);
    if let Err(e) = shared::logger::init_logger(log_level){
        error!("fail to set log level {:?}", e);
        return ResultCode::Panic;
    }
    ResultCode::Success
}
pub unsafe extern "C" fn destroy_game() -> ResultCode {
    crate::game::config::unload();
    shared::logger::clear();
    ResultCode::Success
}
pub extern "C" fn begin_play() -> ResultCode {
    let r = std::panic::catch_unwind(|| {
        //TODO
        crate::game::config::load_configs("configs").unwrap();
    });
    match r {
        Ok(_) => ResultCode::Success,
        Err(_) => ResultCode::Panic,
    }
}
pub extern "C" fn end_play(reason: REndPlayReason) -> ResultCode {
    debug!("end_play {:?}", reason);
    let r = std::panic::catch_unwind(|| {
        //TODO
    });
    match r {
        Ok(_) => ResultCode::Success,
        Err(_) => ResultCode::Panic,
    }
}
pub extern "C" fn on_pause(paused: bool) -> ResultCode {
    debug!("on_pause {}", paused);
    let r = std::panic::catch_unwind(|| {
        //TODO
    });
    match r {
        Ok(_) => ResultCode::Success,
        Err(_) => ResultCode::Panic,
    }
}
pub unsafe extern "C" fn on_anim_notify(
    unit: *const FUnitState,
    duration: f32,
    notify_id: i32,
    notify_value: i32,
) -> ResultCode {
    //info!("object {:?}, notify {}, {}", AActor::from_ptr((*unit).owner).map(|a| a.GetActorNameOrLabel()), duration, notify_id);
    let ret = std::panic::catch_unwind(||{
        AnimNofityManager::on_event(unit, duration, notify_id, notify_value)
    });
    match ret {
        Ok(_) => ResultCode::Success,
        Err(e) => {
            error!("panic {:?}", e);
            ResultCode::Panic
        },
    }
}
pub(crate) fn build_bindings() -> RustBindings{    
    RustBindings {
        // retrieve_uuids: retrieve_uuids,
        log_level: set_log_level,
        init: init_native,
        destroy: destroy_game,
        tick: tick,
        begin_play: begin_play,
        unreal_event: unreal_event,
        end_play: end_play,
        on_pause: on_pause,
        on_game_input: on_game_input,
        anim_fns: AnimationFns{
            notify_fn: on_anim_notify,
        },
        // reflection_fns: engine::create_reflection_fns(),
        // allocate_fns: engine::create_allocate_fns(),
    }
}