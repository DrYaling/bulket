use std::ops::Add;

use glam::Vec3;

use crate::*;

impl Vector3{
    #[inline]
    pub const fn new(x: f32, y: f32, z: f32) -> Self{
        Self { x, y, z }
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
///save int into f32
pub unsafe fn i32_to_f32(i: i32) -> f32{
    let fptr: *const f32 = &i as *const i32 as *const _;
    *fptr
}
impl AnimStateParam{
    #[inline]
    fn bool_to_v3(valid: bool) -> Vector3{
        let iv = if_else!(valid, 1, 0);
        Self::i32_to_vec3(iv)
    }
    #[inline]
    fn i32_to_vec3(i: i32) -> Vector3{
        unsafe{Vector3::new(i32_to_f32(i), 0.0, 0.0)}
    }
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
            value: Self::bool_to_v3(valid)
        }
    }
    #[inline]
    pub fn i32(i: i32) -> Self{
        Self { 
            param_type: EAnimParamType::Integer, 
            value: Self::i32_to_vec3(i)
        }
    }
    #[inline]
    pub fn f32(f: f32) -> Self{
        Self { 
            param_type: EAnimParamType::Float, 
            value: Self::f32_to_v3(f)
        }
    }
    #[inline]
    pub fn vec3(v3: Vec3) -> Self{
        Self { 
            param_type: EAnimParamType::Vector, 
            value: v3.into()
        }
    }

}
///get string hash
pub fn get_name_hash<T: AsRef<str>>(name: T) -> i32{
    let name_str = name.as_ref();
    return name_str.chars().fold(0i32,|c, v|{        
        ((5 * (c as i64) + (v as i64)) % (i32::MAX as i64)) as i32
    });
}