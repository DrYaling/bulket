#include "FFITypes.h"
#include "GamePlayFFI.h"
#include "../RustGameInstance.h"
#include "../Animation/RustAnimInstance.h"
#include "..//FFI/RustApi.h"
#include "..//GameConfig.h"
#include "../FFI/Binder.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "../GameUnit/RustActor.h"
void SetFixedAnimState(int32 ObjectUUID, uint8/*EFixeAnimState*/ FixedState, AnimStateParam Value) {
	if (const auto SGame = sGameInstance) {
		if (const auto Object = SGame->GetGameObject(ObjectUUID)) {
			if (auto Animation = Object->GetUnitState().Animation) {
				FAnimParams Param;
				Param.SetBool((int)(Value.IValue) != 0);
				Animation->SetFixedAnimState((EFixeAnimState)FixedState, Param);
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
				Param.ParamType = (EAnimParamType)Value.ParamType;
				switch (Param.ParamType)
				{
				case EAnimParamType::Bool:
					Param.SetBool(Value.IValue != 0);
					break;
				case EAnimParamType::Float:
					Param.SetFloat(Value.Value.x);
					break;
				case EAnimParamType::Integer:
					Param.SetInt((int)(Value.IValue));
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

void SetHitState(int32 ObjectUUID, AnimHitState State)
{
	if (const auto SGame = sGameInstance) {
		if (const auto Object = SGame->GetGameObject(ObjectUUID)) {
			if (auto Animation = Object->GetUnitState().Animation) {
				FAnimHitState UHitState;
				UHitState.IsHit = true;
				UHitState.HitDirection = ToFVector(State.Direction);
				UHitState.Location = ToFVector(State.Location);
				UHitState.SourceActor = ToAActor(State.SourceActor);
				Animation->SetHitState(UHitState);
			}
		}
	}
}
void SetAnimPlayRate(void* AnimIns, float Rate, float Duration)
{
	if(AnimIns && sGameInstance)
	{
		if(const auto AnimInstance = Cast<URustAnimInstance>((UAnimInstance*)AnimIns))
		{
			AnimInstance->SetAnimPlayRate(Rate, Duration);
		}
	}
}

void GameEventNotify(EGameNotifyEventType EventType, int32 SubType, int64 Value, int64 ValueExt)
{
	if (const auto SGame = sGameInstance) {
		if (auto MainUI = SGame->GetGameMainUI()) {
			MainUI->OnGameEvent(EventType, SubType, Value, ValueExt);
		}
		else {
			UE_LOG(LogNative, Warning, TEXT("UIMain Not Loaded"));
		}
	}
}

ComponentInfo GetActorComponent(void* Actor, EGameComponentType ComponentType)
{
	ComponentInfo info;
	info.ComponentType = ComponentType;
	if (const auto Owner = ToAActor(Actor)) {
		switch (ComponentType)
		{
		case EGameComponentType::CharacterMovement:
		{
			TArray<UCharacterMovementComponent*> Array;
			Owner->GetComponents(Array);
			if (Array.Num() > 0) {
				info.Component = Array[0];
			}
		}
			break;
		case EGameComponentType::SkeletalMesh:
		{
			TArray<USkeletalMeshComponent*> Array;
			Owner->GetComponents(Array);
			if (Array.Num() > 0) {
				info.Component = Array[0];
			}
		}
			break;
		case EGameComponentType::Camera:
		{
			TArray<UCameraComponent*> Array;
			Owner->GetComponents(Array);
			if (Array.Num() > 0) {
				info.Component = Array[0];
			}
		}
			break;
		case EGameComponentType::Capsule:
		{
			TArray<UCapsuleComponent*> Array;
			Owner->GetComponents(Array);
			if (Array.Num() > 0) {
				info.Component = Array[0];
			}
		}
			break;
		case EGameComponentType::Rust:
		{
			TArray<URustComponent*> Array;
			Owner->GetComponents(Array);
			if (Array.Num() > 0) {
				info.Component = Array[0];
			}
		}
		break;
		default:
			break;
		}
	}
	return info;
}


void SetAnimFreeze(void* AnimInstance, bool Freezing, float Duration, bool OverrideDuration)
{
	UAnimInstance* Anim = (UAnimInstance*)AnimInstance;
	if(const auto RAnim = Cast<URustAnimInstance>(Anim))
	{
		if(Freezing)
		{
			RAnim->SetFreeze(Duration, OverrideDuration);
		}
		else
		{
			RAnim->SetNotFreeze();
		}

	}
}
