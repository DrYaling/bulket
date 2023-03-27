#pragma once
#include "CoreMinimal.h"
#include "FFITypes.h"
struct AnimStateParam {
    uint8/*EAnimParamType*/ ParamType;
    int32 IValue;
    Vector3 Value;
};
struct AnimHitState {
    Vector3 Location;
    Vector3 Direction;
    void* SourceActor;
};
using GamePlaySetFixAnimState = void(*)(int32 UUID, uint8/*EFixeAnimState*/ FixedState, AnimStateParam Value);
using GamePlaySetDynAnimState = void(*)(int32 UUID, UName AnimName, AnimStateParam Value);
using GamePlayGetFName = UName(*)(NativeString utf8str);
using GamePlayFNameToString = const char* (*)(UName Name);
using GamePlayGetMontage = void* (*)(UName);
using GamePlaySetMovable = void (*)(int32, bool);
using GamePlaySetHitState = void (*)(int32, AnimHitState);
using GamePlaySetAnimRate = void (*)(void*, float, float);
using GamePlayEventNotify = void (*)(EGameNotifyEventType, int32, int64, int64);
using GamePlayGetComponent = ComponentInfo(*)(void*, EGameComponentType);
using GamePlaySetFreeze = void(*)(void*, bool Freezing, float Duration, bool OverrideDuration);
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
    GamePlaySetHitState set_hit_state;
    GamePlaySetAnimRate set_anim_rate;
    GamePlayEventNotify game_event_notify;
    GamePlayGetComponent get_component;
    GamePlaySetFreeze set_anim_freeze;
    static GameplayFns Create();
};
/*
 * game play extern fns
 */
extern "C"{

    /*
    * set fixed animation state value
    */
    extern void SetFixedAnimState(int32 ObjectUUID, uint8/*EFixeAnimState*/ FixedState, AnimStateParam Value);
    /*
    * set dynamic animation state value
    */
    extern void SetDynAnimState(int32 ObjectUUID, UName AnimName, AnimStateParam Value);
    extern UName CreateUnrealName(NativeString NameString);
    extern const char* UNameToString(UName Name);
    extern void* GetMontage(UName Name);
    extern void SetMovable(int32 ObjectUUID, bool Movable);
    extern void SetHitState(int32 ObjectUUID, AnimHitState State);
    extern void SetAnimPlayRate(void* AnimationInstance, float Rate, float Duration);
    void GameEventNotify(EGameNotifyEventType EventType, int32 SubType, int64 Value, int64 ValueExt);
    ComponentInfo GetActorComponent(void* Actor, EGameComponentType ComponentType);
    void SetAnimFreeze(void* AnimInstance, bool Freezing, float Duration, bool OverrideDuration);
}