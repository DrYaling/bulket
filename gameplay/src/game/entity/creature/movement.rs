use crate::game::entity::IMotion;

use super::*;

pub struct CreatureMotionInfo{
    can_move: bool,
}
impl CreatureMotionInfo{
    pub fn new() -> Self{
        Self { can_move: true }
    }
}
impl IMotion for Creature{
    fn set_movable(&mut self, movable: bool){
        self.motion.can_move = movable;
        engine_api::set_movable(self.uuid(), movable);
    }
    #[inline]
    fn movable(&self) -> bool { self.motion.can_move }
    #[inline]
    fn add_force_bone(&mut self, force: glam::Vec3, bone: engine_api::UName, use_acces: bool) {
        self.creature.anim_instance.GetSkelMeshComponent().map(|mut comp|{
            //好像不生效
            comp.AddImpulseToAllBodiesBelow(force.into(), bone, use_acces, true)
        }).or_else(||{
            error!("monster {}with no movement base ", self.uuid());
            None
        });
    }
}