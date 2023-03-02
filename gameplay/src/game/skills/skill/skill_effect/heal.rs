
/**
 * heal effect
 */
#[derive(Clone)]
pub struct HeadEffect{
    pub value: u32,
    pub tick: u32,
    pub interval: u32,
    pub duration: u32,
    pub heal_on_startup: bool,
}