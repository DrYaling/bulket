use crate::UName;

#[derive(Debug, Clone)]
pub struct PlayAnimInfo{
    pub montage: UName,
    pub speed: f32,
    pub section: Option<UName>,
}
impl PlayAnimInfo{
    pub fn create() -> Self{
        Self { montage: UName::none(), speed: 1.0, section: None }
    }
    pub fn with_name(mut self, montage: UName) -> Self{
        self.montage = montage;
        self
    }
    pub fn with_section(mut self, section: UName) -> Self{
        self.section = Some(section);
        self
    }
    pub fn with_name_str(mut self, name: &str) -> Self{
        self.montage = UName::new(name);
        self
    }
    pub fn with_speed(mut self, speed: f32) -> Self{
        self.speed = speed;
        self
    }

}
#[derive(Debug, Clone)]
pub struct Animation{
    
}