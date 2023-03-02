// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "RustAnimNotifyState.generated.h"

USTRUCT(BlueprintType)
struct FAnimNotifyValue
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 NotifyId;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 Value;
};
/**
 * rust animation notify state, used for native events
 */
UCLASS()
class PROJECTU_API URustAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
protected:
	virtual void PostInitProperties() override;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;
public:
	/**
	 * if this event can be blocked by native 
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool BlockNotify;

	/**
	 * notify value
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FAnimNotifyValue NotifyValue;

private:
	/**
	 * if this event will be blocked by native
	 */
	bool BlockNotifyState;
};
