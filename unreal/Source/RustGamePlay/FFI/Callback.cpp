#include "FFITypes.h"
#include "GamePlayFFI.h"
#include "../RustGameInstance.h"
#include "../RustApi.h"
#include "..//GameConfig.h"
#include "../Binder.h"
void SetFixedAnimState(int32 ObjectUUID, EFixeAnimState FixedState, AnimStateParam Value) {
	if (const auto SGame = sGameInstance) {
		if (const auto Object = SGame->GetGameObject(ObjectUUID)) {
			if (auto Animation = Object->GetUnitState().Animation) {
				FAnimParams Param;
				Param.SetBool((int)(*&Value.Value.x) != 0);
				Animation->SetFixedAnimState(FixedState, Param);
			}
		}
	}
}
void SetDynAnimState(int32 ObjectUUID, UName AnimName, AnimStateParam Value) {
	if (const auto SGame = sGameInstance) {
		const auto Name = ToFName(AnimName);
		if (const auto Object = SGame->GetGameObject(ObjectUUID)) {
			if (auto Animation = Object->GetUnitState().Animation) {
				FAnimParams Param;
				Param.ParamType = Value.ParamType;
				switch (Value.ParamType)
				{
				case EAnimParamType::Bool:
					Param.SetBool((int)(*&Value.Value.x) != 0);
					break;
				case EAnimParamType::Float:
					Param.SetFloat(Value.Value.x);
					break;
				case EAnimParamType::Integer:
					Param.SetInt((int)(*&Value.Value.x));
					break;
				case EAnimParamType::Vector:
					Param.SetVector(FVector(Value.Value.x, Value.Value.y, Value.Value.z));
					break;
				default:
					break;
				}
				Animation->SetDynAnimState(Name, Param);
			}
		}
		else
		{
			UE_LOG(LogNative, Error, TEXT("Object %d  %d not found"), ObjectUUID, *Name.ToString());
		}
	}
}

UName CreateUnrealName(NativeString NameString)
{
	const FName Name = Utf82FName(NameString);
	return ToUName(Name);
}

const char* UNameToString(UName Name)
{
	return FName2Utf8(ToFName(Name));
}

void* GetMontage(UName Name)
{
	if (const auto SGame = sGameInstance)
	{
		const auto UEName = ToFName(Name);
		return SGame->GetGameConfig()->GetMontage(UEName);
	}
	return nullptr;
}

void SetMovable(int32 ObjectUUID, bool Movable)
{
	if (const auto SGame = sGameInstance)
	{
		const auto Object = SGame->GetObjectByUUID(ObjectUUID);
		if (nullptr != Object)
			Object->SetMovable(Movable);
		}
}
