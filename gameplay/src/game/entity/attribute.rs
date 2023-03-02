use std::collections::HashMap;

///attribute type will display in blueprint
#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum EAttributeType{
    None = 0,
    ///精气-感知-术法
    Senseless,
    ///血气-外循环
    BloodAura,
    ///神-内外循环-罡气和回复
    Circulating,    
    Health,
    MaxHealth,
    Mana,
    MaxMana,
    Critical,
    CriticalDamage,
    Hast,
    Speed,
    MaxCount
}
/*
object attibutes
 */
#[derive(Default, Debug, Clone)]
pub struct Attribute{
    attr: [i32; EAttributeType::MaxCount as usize + 1],
    //max 64 attributes
    dirty_flag: u64,    
}
impl Attribute{
    #[inline]
    pub const fn new() -> Self{
        Attribute { attr: [0; EAttributeType::MaxCount as usize + 1], dirty_flag: 0 }
    }
    #[inline]
    pub fn get(&self, attr_type: EAttributeType) -> i32{
        self.attr[attr_type as usize]
    }
    #[inline]
    pub fn set(&mut self, attr_type: EAttributeType, value: i32){
        let index = attr_type as usize;
        let old = self.attr[index];
        if old != value{
            self.attr[index] = value;
            self.dirty_flag |= 1 << index;
        }
    }
    #[inline]
    pub fn is_dirty(&self, attr_type: EAttributeType) -> bool{
        self.dirty_flag & (1 << (attr_type as usize)) != 0
    }
    #[inline]
    pub fn consume_dirty(&mut self, attr_type: EAttributeType) -> bool{
        let bitflag = 1 << (attr_type as usize);
        let dirty = self.dirty_flag & bitflag;
        self.dirty_flag &= !bitflag;
        dirty != 0
    }
    #[inline]
    pub fn get_health(&self) -> i32{
        self.get(EAttributeType::Health)
    }
    #[inline]
    pub fn set_health(&mut self, value: i32){
        self.set(EAttributeType::Health, value)
    }
    #[inline]
    pub fn update_health(&mut self, delta: i32){
        let old = self.get_health();
        let mut current = old + delta;
        if delta > 0{
            current = current.min(self.max_health());
        }
        else{
            current = current.max(0);
        }
        self.set_health(current)
    }
    #[inline]
    pub fn max_health(&self) -> i32{
        self.get(EAttributeType::MaxHealth)
    }
    #[inline]
    pub fn set_max_health(&mut self, value: i32){
        if value < 0{
            return;
        }
        self.set(EAttributeType::MaxHealth, value)
    }
    #[inline]
    pub fn get_mana(&self) -> i32{
        self.get(EAttributeType::Mana)
    }
    #[inline]
    pub fn set_mana(&mut self, value: i32){
        let old = self.get_mana();
        let mut current = old + value;
        if value > 0{
            current = current.min(self.max_mana());
        }
        else{
            current = 0.max(current);
        }
        self.set(EAttributeType::Mana, value)
    }
    #[inline]
    pub fn max_mana(&self) -> i32{
        self.get(EAttributeType::MaxMana)
    }
    #[inline]
    pub fn set_max_mana(&mut self, value: i32){
        if value < 0{
            return;
        }
        self.set(EAttributeType::MaxMana, value)
    }
}
impl From<&Vec<i32>> for Attribute{
    fn from(value: &Vec<i32>) -> Self {
        let mut _self = Self::default();
        let size = value.len().min(EAttributeType::MaxCount as usize);
        unsafe{
            std::intrinsics::copy(value.as_ptr(), _self.attr.as_mut_ptr(), size);
        }
        _self
    }
}
impl From<&HashMap<EAttributeType,i32>> for Attribute{
    fn from(value: &HashMap<EAttributeType,i32>) -> Self {
        let mut _self = Self::default();
        for (e, v) in value {
            _self.attr[*e as usize] = *v;
        }
        _self
    }
}