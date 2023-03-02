use super::IConfig;

#[derive(Debug, Default, Clone, Serialize, Deserialize)]
pub struct SkillConfig{
    pub id: u32,
    
}
impl IConfig for SkillConfig{
    fn get_id(&self) -> u32 {
        self.id
    }
}