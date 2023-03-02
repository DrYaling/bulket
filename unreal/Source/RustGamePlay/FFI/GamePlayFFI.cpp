#include "GamePlayFFI.h"
#include "../RustGameInstance.h"
#include "../RustApi.h"
#include "..//GameConfig.h"
#include "../Binder.h"

GameplayFns GameplayFns::Create()
{
	GameplayFns GamePlayFns = {};
	GamePlayFns.set_dyn_anim_state = &SetDynAnimState;
	GamePlayFns.set_fixed_anim_state = &SetFixedAnimState;
	GamePlayFns.create_unreal_name = &CreateUnrealName;
	GamePlayFns.name_to_string = &UNameToString;
	GamePlayFns.get_montage = &GetMontage;
	GamePlayFns.set_unit_movable = &SetMovable;
	return GamePlayFns;
}
