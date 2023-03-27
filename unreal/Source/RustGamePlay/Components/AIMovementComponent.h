// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterMovementComponentAsync.h"
#include "GameFramework/PawnMovementComponent.h"
#include "../FFI/FFITypes.h"
#include "AIMovementComponent.generated.h"
class ARustPawn;
/**
 * ai move component
 */
UCLASS()
class PROJECTU_API UAIMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UAIMovementComponent();
	/**
	 * @brief add force to pawn
	 * @param Force vector force
	 * @param bForce ignore controller state
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust| GamePlay")
	void AddForce(const FVector& Force, bool bForce);
	/**
	 * @brief add impulse to actor
	 * @param Force impulse force to add
	 * @param bVelocityChange add impulse velocity
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust| GamePlay")
	void AddImpulse(const FVector& Force, bool bVelocityChange = false);
	/**
	 * @brief add location offset(world space)
	 * @param Offset offset vector
	 * @param Duration move duration
	 * @param SafeMove save move(check collision)
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust| GamePlay")
	void AddLocationOffset(const FVector& Offset, float Duration, bool SafeMove = true);
	/**
	 * @brief set freeze state
	 * @param Duration freeze time
	 * @param OverrideDuration override freezing time duration or additional duration
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust| GamePlay")
	void SetFreeze(float Duration, bool OverrideDuration = true);
	/**
	 * @brief cancle freeze state
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust| GamePlay")
	void SetNotFreeze();

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

private:
	void ApplyAccumulatedForces(float DeltaSeconds);
	void PerformMovement(float DeltaTime);
	void MoveSmooth(const FVector& InVelocity, float DeltaSeconds, FStepDownResult* OutStepDownResult);
	void FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation,
	               const FHitResult* DownwardSweepResult) const;

private:
	uint8 bMoving: 1;
	uint8 bFreezing: 1;
	uint8 bGrounded : 1;
	float MoveDuration;
	float FreezingDuration;
	float AnimationRateDuration;
	float Mass;
	EMovementMode MovementMode;
	UPROPERTY()
	TObjectPtr<ARustPawn> OwnerPawn;
	FVector PendingForceToApply;
	FVector PendingImpulseToApply;
	FFindFloorResult CurrentFloor;
};
