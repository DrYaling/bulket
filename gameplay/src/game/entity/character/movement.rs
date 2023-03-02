use crate::game::entity::IMotion;

use super::*;
/**
 * character motion details
 */
pub struct CharacterMotionInfo{
    can_move: bool,
}
impl CharacterMotionInfo{
    pub fn new() -> Self{
        Self { can_move: true }
    }
}
impl IMotion for Character{
    #[inline]
    fn set_movable(&mut self, movable: bool){
        self.motion.can_move = movable;
        info!("set movable {}", self.motion.can_move);
        engine_api::set_movable(self.uuid(), movable);
    }
    #[inline]
    fn movable(&self) -> bool { self.motion.can_move }
}