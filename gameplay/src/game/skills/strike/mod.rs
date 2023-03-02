//! strike effects
/**
 * strike type
 */
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum EStrikeType {
    None = 0,
    Repel = 1,
    Floating = 2,
    Freezing = 3,
}
/*
strike info 
 */
#[derive(Debug, Clone, Copy)]
pub struct StrikeInfo{
    pub r#type: EStrikeType,
    pub value: i32,
}
impl Default for StrikeInfo{
    fn default() -> Self {
        Self { r#type: EStrikeType::None, value: 0 }
    }
}