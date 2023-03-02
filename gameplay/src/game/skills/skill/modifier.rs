//! skill modifier

use shared::Vector;

use crate::game::{entity::EAttributeType, skills::strike::EStrikeType};
//skill modifier type
#[derive(Debug, Clone)]
pub enum Modifier{
    ///modify attribute this skill take affect 
    Attribute(ModifierInfo<EAttributeType>),
    ///modify strick this skill take affect 
    Strike(ModifierInfo<EStrikeType>),
    ///add some buff
    Buff(Vector<u32>),
    ///modifier that modify other skill(should use buff modifier)
    Skill(Vector<u32>),
    ///increase(value > 0) or decrease(value < 0) cast time in mills(or percent while second value is false)
    CastTime(i32, bool),
    ///increase (value > 0) or decrease (value < 0) skill cooldown in mills(or percent while second value is false)
    CoolTime(i32),
    ///set skill can not be break while casting for time in mills(0 for forever)
    CastProtect(u32),
}
/**
 * skill modifier
 */
#[derive(Debug, Clone)]
pub struct ModifierInfo<T>{
    pub modifier_type: T,
    pub value: i32,
}
#[derive(Debug, Clone)]
pub struct SkillModifier{
    ///modifier
    pub modifier: Modifier,
    ///add time in mills
    pub add_time: u32,
    ///modifier tick
    pub ticker: u32,
    ///expire time in mills, 0 implies this modifier will never expire
    pub expire_time: u32,
}