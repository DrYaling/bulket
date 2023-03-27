// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../GameTypes.h"
#include "Camera/CameraActor.h"
#include "Camera/PlayerCameraManager.h"
#include "Delegates/IDelegateInstance.h"
#include "GameCamera.generated.h"

UCLASS()
class PROJECTU_API AGameCamera : public ACameraActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGameCamera();
private:
	void LateInitialize();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/**
	 * @brief game camera event triggered by game
	 * @param CameraEvent camera event 
	 * @param EventOwner event owner actor
	 * @param Parameter event parameter
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Rust|Camera", meta = (Keywords = "Camera"))
	void OnGameCameraEvent(EGameCameraEvent CameraEvent, AActor* EventOwner, float Parameter);
	
		/**
		 * Set the view target blending with variable control
		 * @param NewViewTarget - new actor to set as camera target
		 * @param BlendTime - time taken to blend
		 * @param BlendFunc - Cubic, Linear etc functions for blending
		 * @param BlendExp -  Exponent, used by certain blend functions to control the shape of the curve.
		 * @param bLockOutgoing - If true, lock outgoing viewtarget to last frame's camera position for the remainder of the blend.
		 */
	UFUNCTION(BlueprintCallable, Category = "Rust|Camera", meta = (Keywords = "Camera"))
	void SetLookAtTargetWithBlend(class AActor* NewViewTarget, float BlendTime = 0, enum EViewTargetBlendFunction BlendFunc = VTBlend_Linear, float BlendExp = 0, bool bLockOutgoing = false);
	UFUNCTION(BlueprintCallable, Category = "Rust|Camera", meta = (Keywords = "Camera"))
	/**
	 * set the look at target
	 */
	void SetLookAtTarget(class AActor* NewViewTarget);
public:
	/****************		public camera settings	**********************/
	/**
	 *runtime player change support
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rust|Camera")
	int32 PlayerIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rust|Camera")
	float CharacterSwitchBlendTime = 0.3;
private:
	uint8 bInitialized = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rust|Camera", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class APlayerController> Controller;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rust|Camera", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class ARustCharacter> CurrentCharacter;
	/**
	 * @brief delegate handler
	 */
		FDelegateHandle DelegateHandle;
};
