// Fill out your copyright notice in the Description page of Project Settings.


#include "AIMovementComponent.h"

#include "../Animation/RustAnimInstance.h"
#include "../GameUnit/RustPawn.h"
#include "Engine/NetDriver.h"
#include "Engine/NetworkObjectList.h"

UAIMovementComponent::UAIMovementComponent()
{
}

void UAIMovementComponent::AddForce(const FVector& Force, bool bForce)
{
	if (!Force.IsZero())
	{
		if (Mass > UE_SMALL_NUMBER)
		{
			PendingForceToApply += Force / Mass;
		}
		else
		{
			UE_LOG(LogNative, Warning, TEXT("Attempt to apply force to zero or negative Mass in UAIMovementComponent"));
		}
	}
}

void UAIMovementComponent::AddImpulse(const FVector& Impulse, bool bVelocityChange)
{
	if (!Impulse.IsZero())
	{
		// handle scaling by mass
		FVector FinalImpulse = Impulse;
		if (!bVelocityChange)
		{
			if (Mass > UE_SMALL_NUMBER)
			{
				FinalImpulse = FinalImpulse / Mass;
			}
			else
			{
				UE_LOG(LogNative, Warning, TEXT("Attempt to apply impulse to zero or negative Mass in UAIMovementComponent"));
			}
		}

		PendingImpulseToApply += FinalImpulse;
	}
}
void UAIMovementComponent::ApplyAccumulatedForces(float DeltaSeconds)
{
	if (PendingImpulseToApply.Z != 0.f || PendingForceToApply.Z != 0.f)
	{
		// check to see if applied momentum is enough to overcome gravity
		if (IsMovingOnGround() && (PendingImpulseToApply.Z + (PendingForceToApply.Z * DeltaSeconds) + (GetGravityZ() * DeltaSeconds) > UE_SMALL_NUMBER))
		{
			MovementMode = MOVE_Falling;
		}
	}

	Velocity += PendingImpulseToApply + (PendingForceToApply * DeltaSeconds);

	// Don't call ClearAccumulatedForces() because it could affect launch velocity
	PendingImpulseToApply = FVector::ZeroVector;
	PendingForceToApply = FVector::ZeroVector;
}

void UAIMovementComponent::AddLocationOffset(const FVector& Offset, float Duration, bool SafeMove)
{
}

void UAIMovementComponent::SetFreeze(float Duration, bool OverrideDuration)
{
	if (Duration <= 0.0)
		return;
	bFreezing = true;
	if(OverrideDuration)
		FreezingDuration = Duration;
	else
		FreezingDuration += Duration;
	if (OwnerPawn)
	{
		OwnerPawn->GetAnimInstance()->SetFreeze(Duration, OverrideDuration);
	}
}

void UAIMovementComponent::SetNotFreeze()
{
	bFreezing = false;
	FreezingDuration = 0.0;
	if (OwnerPawn)
		OwnerPawn->GetAnimInstance()->SetNotFreeze();
}

void UAIMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bFreezing)
	{
		FreezingDuration -= DeltaTime;
		if (FreezingDuration <= 0.0)
		{
			SetNotFreeze();
		}
	}
	if (!bMoving || !OwnerPawn || bFreezing)
		return;
	PerformMovement(DeltaTime);
}

void UAIMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerPawn = Cast<ARustPawn>(GetOwner());
	bMoving = false;
	bFreezing = false;
	FreezingDuration = 0.0;
	MoveDuration = 0.0f;
	if(OwnerPawn && OwnerPawn->GetMesh())
		Mass = OwnerPawn->GetMesh()->GetMass();
}

void UAIMovementComponent::BeginDestroy()
{
	Super::BeginDestroy();
	OwnerPawn = nullptr;
}
//copy
void UAIMovementComponent::PerformMovement(float DeltaSeconds)
{
	//const UWorld* MyWorld = GetWorld();
	//if (!OwnerPawn || MyWorld == nullptr)
	//{
	//	return;
	//}
	//ApplyAccumulatedForces(DeltaSeconds);
	//FStepDownResult StepDownResult;
	//MoveSmooth(Velocity, DeltaSeconds, &StepDownResult);

	//// find floor and check if falling
	//if (IsMovingOnGround() || MovementMode == MOVE_Falling)
	//{
	//	if (StepDownResult.bComputedFloor)
	//	{
	//		CurrentFloor = StepDownResult.FloorResult;
	//	}
	//	else if (Velocity.Z <= 0.f)
	//	{
	//		FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, Velocity.IsZero(), NULL);
	//	}
	//	else
	//	{
	//		CurrentFloor.Clear();
	//	}

	//	if (!CurrentFloor.IsWalkableFloor())
	//	{
	//		if (!bSimGravityDisabled)
	//		{
	//			// No floor, must fall.
	//			if (Velocity.Z <= 0.f || bApplyGravityWhileJumping || !CharacterOwner->IsJumpProvidingForce())
	//			{
	//				Velocity = NewFallVelocity(Velocity, FVector(0.f, 0.f, GetGravityZ()), DeltaSeconds);
	//			}
	//		}
	//		SetMovementMode(MOVE_Falling);
	//	}
	//	else
	//	{
	//		// Walkable floor
	//		if (IsMovingOnGround())
	//		{
	//			AdjustFloorHeight();
	//			SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
	//		}
	//		else if (MovementMode == MOVE_Falling)
	//		{
	//			if (CurrentFloor.FloorDist <= MIN_FLOOR_DIST || (bSimGravityDisabled && CurrentFloor.FloorDist <= MAX_FLOOR_DIST))
	//			{
	//				// Landed
	//				SetPostLandedPhysics(CurrentFloor.HitResult);
	//			}
	//			else
	//			{
	//				if (!bSimGravityDisabled)
	//				{
	//					// Continue falling.
	//					Velocity = NewFallVelocity(Velocity, FVector(0.f, 0.f, GetGravityZ()), DeltaSeconds);
	//				}
	//				CurrentFloor.Clear();
	//			}
	//		}
	//	}
	//}
}

void UAIMovementComponent::MoveSmooth(const FVector& InVelocity, const float DeltaSeconds, FStepDownResult* OutStepDownResult)
{
	// Custom movement mode.
	//// Custom movement may need an update even if there is zero velocity.
	//if (MovementMode == MOVE_Custom)
	//{
	//	FScopedMovementUpdate ScopedMovementUpdate(UpdatedComponent, bEnableScopedMovementUpdates ? EScopedUpdate::DeferredUpdates : EScopedUpdate::ImmediateUpdates);
	//	PhysCustom(DeltaSeconds, 0);
	//	return;
	//}

	//FVector Delta = InVelocity * DeltaSeconds;
	//if (Delta.IsZero())
	//{
	//	return;
	//}

	//FScopedMovementUpdate ScopedMovementUpdate(UpdatedComponent, bEnableScopedMovementUpdates ? EScopedUpdate::DeferredUpdates : EScopedUpdate::ImmediateUpdates);

	//if (IsMovingOnGround())
	//{
	//	MoveAlongFloor(InVelocity, DeltaSeconds, OutStepDownResult);
	//}
	//else
	//{
	//	FHitResult Hit(1.f);
	//	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);

	//	if (Hit.IsValidBlockingHit())
	//	{
	//		bool bSteppedUp = false;

	//		if (IsFlying())
	//		{
	//			if (CanStepUp(Hit))
	//			{
	//				OutStepDownResult = NULL; // No need for a floor when not walking.
	//				if (FMath::Abs(Hit.ImpactNormal.Z) < 0.2f)
	//				{
	//					const FVector GravDir = FVector(0.f, 0.f, -1.f);
	//					const FVector DesiredDir = Delta.GetSafeNormal();
	//					const float UpDown = GravDir | DesiredDir;
	//					if ((UpDown < 0.5f) && (UpDown > -0.2f))
	//					{
	//						bSteppedUp = StepUp(GravDir, Delta * (1.f - Hit.Time), Hit, OutStepDownResult);
	//					}
	//				}
	//			}
	//		}

	//		// If StepUp failed, try sliding.
	//		if (!bSteppedUp)
	//		{
	//			SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, false);
	//		}
	//	}
	//}
}

void UAIMovementComponent::FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult) const
{
	//// No collision, no floor...
	//if (!UpdatedComponent->IsQueryCollisionEnabled())
	//{
	//	OutFloorResult.Clear();
	//	return;
	//}
	//// Increase height check slightly if walking, to prevent floor height adjustment from later invalidating the floor result.
	//const float HeightCheckAdjust = (IsMovingOnGround() ? UCharacterMovementComponent::MAX_FLOOR_DIST + UE_KINDA_SMALL_NUMBER : -UCharacterMovementComponent::MAX_FLOOR_DIST);

	//float FloorSweepTraceDist = FMath::Max(UCharacterMovementComponent::MAX_FLOOR_DIST, MaxStepHeight + HeightCheckAdjust);
	//float FloorLineTraceDist = FloorSweepTraceDist;
	//bool bNeedToValidateFloor = true;

	//// Sweep floor
	//if (FloorLineTraceDist > 0.f || FloorSweepTraceDist > 0.f)
	//{
	//	UCharacterMovementComponent* MutableThis = const_cast<UCharacterMovementComponent*>(this);

	//	if (bAlwaysCheckFloor || !bCanUseCachedLocation || bForceNextFloorCheck || bJustTeleported)
	//	{
	//		MutableThis->bForceNextFloorCheck = false;
	//		ComputeFloorDist(CapsuleLocation, FloorLineTraceDist, FloorSweepTraceDist, OutFloorResult, CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), DownwardSweepResult);
	//	}
	//	else
	//	{
	//		// Force floor check if base has collision disabled or if it does not block us.
	//		UPrimitiveComponent* MovementBase = CharacterOwner->GetMovementBase();
	//		const AActor* BaseActor = MovementBase ? MovementBase->GetOwner() : NULL;
	//		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

	//		if (MovementBase != NULL)
	//		{
	//			MutableThis->bForceNextFloorCheck = !MovementBase->IsQueryCollisionEnabled()
	//				|| MovementBase->GetCollisionResponseToChannel(CollisionChannel) != ECR_Block
	//				|| MovementBaseUtility::IsDynamicBase(MovementBase);
	//		}

	//		const bool IsActorBasePendingKill = BaseActor && !IsValid(BaseActor);

	//		if (!bForceNextFloorCheck && !IsActorBasePendingKill && MovementBase)
	//		{
	//			//UE_LOG(LogCharacterMovement, Log, TEXT("%s SKIP check for floor"), *CharacterOwner->GetName());
	//			OutFloorResult = CurrentFloor;
	//			bNeedToValidateFloor = false;
	//		}
	//		else
	//		{
	//			MutableThis->bForceNextFloorCheck = false;
	//			ComputeFloorDist(CapsuleLocation, FloorLineTraceDist, FloorSweepTraceDist, OutFloorResult, CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), DownwardSweepResult);
	//		}
	//	}
	//}

	//// OutFloorResult.HitResult is now the result of the vertical floor check.
	//// See if we should try to "perch" at this location.
	//if (bNeedToValidateFloor && OutFloorResult.bBlockingHit && !OutFloorResult.bLineTrace)
	//{
	//	const bool bCheckRadius = true;
	//	if (ShouldComputePerchResult(OutFloorResult.HitResult, bCheckRadius))
	//	{
	//		float MaxPerchFloorDist = FMath::Max(MAX_FLOOR_DIST, MaxStepHeight + HeightCheckAdjust);
	//		if (IsMovingOnGround())
	//		{
	//			MaxPerchFloorDist += FMath::Max(0.f, PerchAdditionalHeight);
	//		}

	//		FFindFloorResult PerchFloorResult;
	//		if (ComputePerchResult(GetValidPerchRadius(), OutFloorResult.HitResult, MaxPerchFloorDist, PerchFloorResult))
	//		{
	//			// Don't allow the floor distance adjustment to push us up too high, or we will move beyond the perch distance and fall next time.
	//			const float AvgFloorDist = (UCharacterMovementComponent::MIN_FLOOR_DIST + UCharacterMovementComponent::MAX_FLOOR_DIST) * 0.5f;
	//			const float MoveUpDist = (AvgFloorDist - OutFloorResult.FloorDist);
	//			if (MoveUpDist + PerchFloorResult.FloorDist >= MaxPerchFloorDist)
	//			{
	//				OutFloorResult.FloorDist = AvgFloorDist;
	//			}

	//			// If the regular capsule is on an unwalkable surface but the perched one would allow us to stand, override the normal to be one that is walkable.
	//			if (!OutFloorResult.bWalkableFloor)
	//			{
	//				// Floor distances are used as the distance of the regular capsule to the point of collision, to make sure AdjustFloorHeight() behaves correctly.
	//				OutFloorResult.SetFromLineTrace(PerchFloorResult.HitResult, OutFloorResult.FloorDist, FMath::Max(OutFloorResult.FloorDist, UCharacterMovementComponent::MIN_FLOOR_DIST), true);
	//			}
	//		}
	//		else
	//		{
	//			// We had no floor (or an invalid one because it was unwalkable), and couldn't perch here, so invalidate floor (which will cause us to start falling).
	//			OutFloorResult.bWalkableFloor = false;
	//		}
	//	}
	//}
}