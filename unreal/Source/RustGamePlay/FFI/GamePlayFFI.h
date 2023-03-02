#pragma once
#include "CoreMinimal.h"
#include "../RustAnimInstance.h"
#include "FFITypes.h"
struct AnimStateParam {
    EAnimParamType ParamType;
    Vector3 Value;
};
using GamePlaySetFixAnimState = void(*)(int32 UUID, EFixeAnimState FixedState, AnimStateParam Value);
using GamePlaySetDynAnimState = void(*)(int32 UUID, UName AnimName, AnimStateParam Value);
using GamePlayGetFName = UName(*)(NativeString utf8str);
using GamePlayFNameToString = const char* (*)(UName Name);
using GamePlayGetMontage = void* (*)(UName);
using GamePlaySetMovable = void (*)(int32, bool);
/*
* game play callbacks
*/
struct GameplayFns {
    /*
    * set fixed animation state value
    */
    GamePlaySetFixAnimState set_fixed_anim_state;
    /*
    * set dynamic animation state value
    */
    GamePlaySetDynAnimState set_dyn_anim_state;
    GamePlayGetFName create_unreal_name;
    GamePlayFNameToString name_to_string;
    GamePlayGetMontage get_montage;
    GamePlaySetMovable set_unit_movable;
    static GameplayFns Create();
};
/*
 * game play extern fns
 */
extern "C"{

    /*
    * set fixed animation state value
    */
    extern void SetFixedAnimState(int32 ObjectUUID, EFixeAnimState FixedState, AnimStateParam Value);
    /*
    * set dynamic animation state value
    */
    extern void SetDynAnimState(int32 ObjectUUID, UName AnimName, AnimStateParam Value);
    extern UName CreateUnrealName(NativeString NameString);
    extern const char* UNameToString(UName Name);
    extern void* GetMontage(UName Name);
    extern void SetMovable(int32 ObjectUUID, bool Movable);

}