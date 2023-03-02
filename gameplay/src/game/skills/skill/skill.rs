use shared::Vector;

use crate::game::skills::strike::StrikeInfo;

use super::{modifier::{Modifier, SkillModifier}, skill_effect::SkillEffect};


/**
 * skill info
 */
#[derive(Default)]
pub struct SkillInfo{
    ///can move when skill was casting
    pub(super) movable: bool,
    /// does haste effect cool time?
    pub(super) haste_effect_cool: bool,
    ///cast can be interrupted
    pub(super) can_be_break: bool,
}
impl SkillInfo{
    pub fn new(skill_id: u32) -> Self{
        todo!()
    }
}
/**
 * skill state
 */
#[derive(Debug, Clone)]
pub enum SkillState{
    ///ready to cast
    Ready,
    ///can not cast at this point, value is the cooling time(mills) left
    Cooling(u32),
    ///is casting, value is cast left time in mills
    Casting(u32),
    ///skill was disabled to cast for time in mills, 0 mean forever disabled
    Disabled(u32),
}
/**
 * skill
 */
pub struct Skill{
    skill_id: u32,
    pub(super) skill_cd: u32, 
    ///cast time in mills
    pub(super) cast_time: u32,
    ///default cast time in mills,
    pub(super) def_cast_time: u32,
    skill_state: SkillState,
    pub(super) strikes: Vector<StrikeInfo>,
    pub(super) skill_modifier: Vector<SkillModifier>,
    pub(super) effects: Vector<SkillEffect>
}
impl Skill{
    pub fn new(skill_id: u32) -> Option<Self>{
        None
    }
}