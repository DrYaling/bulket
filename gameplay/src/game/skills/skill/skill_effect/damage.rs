#[derive(Debug, Clone, PartialEq, Eq)]
pub enum DamageType{
    Physics,
    Magic,
}
/**
 * damage effect
 */
#[derive(Clone)]
pub struct Damage{
    pub value: u32,
    pub tick: u32,
    pub interval: u32,
    pub duration: u32,
    pub damage_type: DamageType,
    pub damage_on_startup: bool,
}