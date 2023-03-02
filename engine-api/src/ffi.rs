//! ffi apis with unreal
use crate::{engine};

use super::binders::*;
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
}