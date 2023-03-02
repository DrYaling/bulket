mod engine;
mod unreal;
mod wrapper;
mod physics;
mod game_api;
pub use engine::*;
pub use unreal::*;
pub use wrapper::*;
pub use physics::*;
pub use game_api::*;
use crate::*;
///	Called when this Actor hits (or is hit by) something solid. This could happen due to things like Character movement, using Set Location with 'sweep' enabled, or physics simulation.
/// 
///	For events when objects overlap (e.g. walking into a trigger) see the 'Overlap' event.
/// 
///	@note For collisions during physics simulation to generate hit events, 'Simulation Generates Hit Events' must be enabled.
pub fn register_on_hit(actor: &AActor){
    unsafe {
        (engine::bindings().actor_fns.register_actor_on_hit)(actor.inner())
    }
}
/** 
 * Called when another actor begins to overlap this actor, for example a player walking into a trigger.
    * For events when objects have a blocking collision, for example a player hitting a wall, see 'Hit' events.
    * @note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
    */
pub fn register_overlap(actor: &AActor){
    unsafe{
         (engine::bindings().actor_fns.register_actor_on_overlap)(actor.inner())
    }
}
pub fn spawn_actor(bp_name: &str, transform: Transform, actor_type: ActorClass) -> Option<AActor>{
    unsafe{        
		string_2_cstr!(bp_name, bp_name);
        AActor::from_ptr((engine::bindings().spawn_bp_actor)(bp_name, transform, actor_type))
    }
}