// Fill out your copyright notice in the Description page of Project Settings.


#include "RustAnimNotifyState.h"
#include "../GameUnit/RustActor.h"
#include "../RustGameInstance.h"
#include "../FFI/RustApi.h"
#include "../GameUnit/RustCharacter.h"
#include "../GameUnit/RustPawn.h"
#include "../FFI/RustPlugin.h"
#include "../Skill/SkillAsset.h"
#include "../TransformLayer.h"
/**
 * get owner game unit info
 */
const FUnitState& GetGameOwnerState(const AActor* Owner)
{
	if (const auto RustObject = Cast<IRustObjectInterface>(Owner))
	{
		return RustObject->GetUnitState();
	}
	const auto Parent = Owner->GetParentActor();
	if(Parent != nullptr)
	{
		return GetGameOwnerState(Parent);
	}
	return FUnitState::Default();
}
void URustAnimNotifyState::PostInitProperties()
{
	Super::PostInitProperties();
	UE_LOG(LogNative, Display, TEXT("PostInitProperties"));
	BlockNotifyState = false;
	if (!BlockNotify)
		return;
	if (const auto SGame = sGameInstance)
		BlockNotifyState = !SGame->IsGameServer();
}

void URustAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if(!BlockNotifyState)
	{
		Received_NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	}
	else
	{
		if(SkillInstance)
		{
			UE_LOG(LogNative, Display, TEXT("BeginNotify %s"), *MeshComp->GetName());
			FAnimNotifyValue Value = NotifyValue;
			Value.IsBegin = true;
			SkillInstance->OnAnimNotify(Value);

		}
		//const auto UnitState = GetGameOwnerState(MeshComp->GetOwner());
		//if(UnitState.UnitType != ERustUnitType::Undefined)
		//{
		//	const FAnimNotifyValue Value = NotifyValue;
		//	//object,  duration, notify value;
		//	if(const auto SGame = sGameInstance)
		//	{
		//		SGame->GetPlugin()->Rust.anim_fns.notify_fn(&UnitState, TotalDuration, Value.NotifyId, Value.Value);
		//	}
		//}
	}
}

void URustAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	//UE_LOG(LogNative, Display, TEXT("NotifyTick %s"), *MeshComp->GetName());
	if (!BlockNotifyState)
		Received_NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void URustAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (!BlockNotifyState)
		Received_NotifyEnd(MeshComp, Animation, EventReference);
	else
	{
		if (SkillInstance && NotifyValue.NotifyType == EAnimNotifyType::SkillDamageCheck)
		{
			UE_LOG(LogNative, Display, TEXT("NotifyEnd %s"), *MeshComp->GetName());
			FAnimNotifyValue Value = NotifyValue;
			Value.IsBegin = false;
			SkillInstance->OnAnimNotify(Value);
		}
	}
}

void URustAnimNotifyState::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);
}

void URustAnimNotifyState::BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload,
	float FrameDeltaTime)
{
	Super::BranchingPointNotifyTick(BranchingPointPayload, FrameDeltaTime);
}

void URustAnimNotifyState::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);
}
