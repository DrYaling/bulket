mod animation;
pub use animation::*;
use crate::{UAnimMontage, IPtr, ACharacter, AnimStateParam, UName, EFixeAnimState, UAnimInstance};

/// play montage for character
pub fn play_montage(character: &mut ACharacter, anim: PlayAnimInfo) -> bool{
    if let Some(mut montage) = UAnimMontage::from_ptr(unsafe{(crate::bindings().gameplay_fns.get_montage)(anim.montage)} as *mut _){
        let len = character.PlayAnimMontage(&mut montage, anim.speed, anim.section.unwrap_or_default());
        return len > 0.0;
    }
    false
}
///play montage for animation instance
pub fn play_anim_montage(anim_instance: &mut UAnimInstance, anim: PlayAnimInfo) -> bool{
    if let Some(mut montage) = UAnimMontage::from_ptr(unsafe{(crate::bindings().gameplay_fns.get_montage)(anim.montage)} as *mut _){
        let len = anim_instance.Montage_Play(
            &mut montage, 
            anim.speed, 
            crate::EMontagePlayReturnType::MontageLength,
            0.0,
            false,
        );
        if len > 0.0{
            if let Some(section) = anim.section{
                anim_instance.Montage_JumpToSection(section, &mut montage)
            }
		}
        return len > 0.0;
    }
    false
}
///设置动画状态
pub fn set_anim_state(unit: i32, anim_name: UName, state: AnimStateParam) -> bool{
    unsafe{
        (crate::bindings().gameplay_fns.set_dyn_anim_state)(unit, anim_name, state);
        true
    }
}
///设置动画状态
pub fn set_fixed_state(unit: i32, fixed_state: EFixeAnimState, state: AnimStateParam) -> bool{
    unsafe{
        (crate::bindings().gameplay_fns.set_fixed_anim_state)(unit, fixed_state, state);
        true
    }
}