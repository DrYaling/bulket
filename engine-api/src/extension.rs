use std::ops::{Add, Neg};

use glam::Vec3;

use crate::*;

impl Vector3{
    #[inline]
    pub const fn new(x: f32, y: f32, z: f32) -> Self{
        Self { x, y, z }
    }
    #[inline]
    pub const fn to_vec(self) -> Vec3{
        Vec3::new(self.x, self.y, self.z)
    }
}
impl Add for Vector3{
    type Output = Vector3;
    #[inline]
    fn add(self, rhs: Self) -> Self::Output {
        Vector3{
            x: self.x + rhs.x,
            y: self.y + rhs.y,
            z: self.z + rhs.z,
        }
    }
}
impl Neg for Vector3{
    type Output = Vector3;
    #[inline]
    fn neg(self) -> Self::Output {
        Vector3{
            x: -self.x,
            y: -self.y,
            z: -self.z,
        }
    }
}
impl AnimStateParam{
    #[inline]
    fn f32_to_v3(f: f32) -> Vector3{
        Vector3::new(f, 0.0, 0.0)
    }
    #[inline]
    pub fn new() -> Self{
        Self::bool(false)
    }
    #[inline]
    pub fn bool(valid: bool) -> Self{
        Self { 
            param_type: EAnimParamType::Bool, 
            value: Vector3::default(),
            ivalue: if_else!(valid, 1, 0),
        }
    }
    #[inline]
    pub fn i32(i: i32) -> Self{
        Self { 
            param_type: EAnimParamType::Integer, 
            value: Vector3::default(),
            ivalue: i,
        }
    }
    #[inline]
    pub fn f32(f: f32) -> Self{
        Self { 
            param_type: EAnimParamType::Float, 
            value: Self::f32_to_v3(f),
            ivalue: 0,
        }
    }
    #[inline]
    pub fn vec3(v3: Vec3) -> Self{
        Self { 
            param_type: EAnimParamType::Vector, 
            value: v3.into(),
            ivalue: 0,
        }
    }

}
impl AActor{
    pub fn as_uobject(&self) -> UObject{
        UObject::from_ptr(self.inner()).unwrap() 
    }
    pub fn get_transform(&self) -> Transform{
        if let Some(root) = self.GetRootComponent(){
            root.GetRelativeTransform()
        }
        else{
            Transform::default()
        }
    }
}
impl APawn{
    pub fn as_actor(&self) -> AActor{
        AActor::from_ptr(self.inner()).unwrap()
    }
}
impl ACharacter{
    pub fn as_actor(&self) -> AActor{
        AActor::from_ptr(self.inner()).unwrap()
    }
    pub fn as_pawn(&self) -> APawn{
        APawn::from_ptr(self.inner()).unwrap()
    }
}
impl USceneComponent{
    pub fn as_comp(&self) -> UActorComponent{
        UActorComponent::from_ptr(self.inner()).unwrap()
    }
}
impl UName{
    #[inline]
    pub fn new(name: &str) -> Self{
        super::string_2_cstr!(name, name);
        let _self = unsafe{(engine::bindings().gameplay_fns.create_unreal_name)(name)};
        _self
    }
    #[inline]
    pub fn to_str(&self) -> String{
        char_str_2_string(unsafe{(engine::bindings().gameplay_fns.name_to_string)(*self)})
    }
    //same as FName::NAME_None
    #[inline]
    pub fn none() -> Self{
        Self::default()
    }
}
impl USkeletalMeshComponent{
    #[inline]
    pub fn as_primitive(&self) -> UPrimitiveComponent{
        UPrimitiveComponent::from_ptr(self.inner()).unwrap()
    }
}
impl UCharacterMovementComponent{
    #[inline]
    pub fn as_actor_comp(&self) -> UActorComponent{
        UActorComponent::from_ptr(self.inner()).unwrap()
    }
    #[inline]
    pub fn as_movement(&self) -> UMovementComponent{
        UMovementComponent::from_ptr(self.inner()).unwrap()
    }
}
///get string hash
pub fn get_name_hash<T: AsRef<str>>(name: T) -> i32{
    let name_str = name.as_ref();
    return name_str.chars().fold(0i32,|c, v|{        
        ((5 * (c as i64) + (v as i64)) % (i32::MAX as i64)) as i32
    });
}