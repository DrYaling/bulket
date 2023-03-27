#include "GamePlayFFI.h"
#include "../RustGameInstance.h"
#include "../FFI/RustApi.h"
#include "..//GameConfig.h"
#include "../FFI/Binder.h"

GameplayFns GameplayFns::Create()
{
	GameplayFns GamePlayFns = {};
	GamePlayFns.set_dyn_anim_state = &SetDynAnimState;
	GamePlayFns.set_fixed_anim_state = &SetFixedAnimState;
	GamePlayFns.create_unreal_name = &CreateUnrealName;
	GamePlayFns.name_to_string = &UNameToString;
	GamePlayFns.get_montage = &GetMontage;
	GamePlayFns.set_unit_movable = &SetMovable;
	GamePlayFns.set_hit_state = &SetHitState;
	GamePlayFns.set_anim_rate = &SetAnimPlayRate;
	GamePlayFns.game_event_notify = &GameEventNotify;
	GamePlayFns.get_component = &GetActorComponent;
	GamePlayFns.set_anim_freeze = &SetAnimFreeze;
	return GamePlayFns;
}
