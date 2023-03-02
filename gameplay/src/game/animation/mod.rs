mod animation;
pub use animation::*;
use engine_api::{UAnimMontage, IPtr, ACharacter, AnimStateParam, UName, EFixeAnimState, UAnimInstance};

use super::entity::IUnit;
/// play montage for character
pub fn play_montage(character: &mut ACharacter, anim: PlayAnimInfo) -> bool{
    if let Some(mut montage) = UAnimMontage::from_ptr(unsafe{(engine_api::bindings().gameplay_fns.get_montage)(anim.montage)} as *mut _){
        let len = character.PlayAnimMontage(&mut montage, anim.speed, anim.section.unwrap_or_default());
        return len > 0.0;
    }
    false
}
///play montage for animation instance
pub fn play_anim_montage(anim_instance: &mut UAnimInstance, anim: PlayAnimInfo) -> bool{
    if let Some(mut montage) = UAnimMontage::from_ptr(unsafe{(engine_api::bindings().gameplay_fns.get_montage)(anim.montage)} as *mut _){
        let start_time = if let Some(section) = anim.section{
            // montage.ge
            //TODO
            //find section start time 
            0.0
        }
        else{
            0.0
        };
        let len = anim_instance.Montage_Play(
            &mut montage, 
            anim.speed, 
            engine_api::EMontagePlayReturnType::MontageLength,
            start_time,
            false,
        );
        /*
        	if (Duration > 0.f)
		{
			// Start at a given Section.
			if( StartSectionName != NAME_None )
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
			}

			return Duration;
		}
         */
        return len > 0.0;
    }
    false
}
///设置动画状态
pub fn set_anim_state<TUnit: IUnit>(unit: &TUnit, anim_name: UName, state: AnimStateParam) -> bool{
    unsafe{
        (engine_api::bindings().gameplay_fns.set_dyn_anim_state)(unit.uuid(), anim_name, state);
        true
    }
}
///设置动画状态
pub fn set_fixed_state<TUnit: IUnit>(unit: &TUnit, fixed_state: EFixeAnimState, state: AnimStateParam) -> bool{
    unsafe{
        (engine_api::bindings().gameplay_fns.set_fixed_anim_state)(unit.uuid(), fixed_state, state);
        true
    }
}