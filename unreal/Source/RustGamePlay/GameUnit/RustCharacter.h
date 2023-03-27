// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "RustActor.h"
#include "../FFI/RustApi.h"
#include "../Components/RustProperty.h"
#include "../Skill/SkillAsset.h"
#include "RustCharacter.generated.h"
UENUM(BlueprintType)
enum class ERustCharacterControllerType: uint8
{
	None,
	Player,
	Ai,
};
UCLASS()
class PROJECTU_API ARustCharacter : public ACharacter, public IRustObjectInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARustCharacter(const FObjectInitializer& ObjectInitializer);
	void InitAsNewSpawned();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual FUnitState& GetUnitState() override { return UnitState; }
	// get all skills
	TMap<int32, USkillAsset*>const& GetSkillAssets() const { return SkillAssets; }
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	virtual void UnPossessed() override;
	virtual void Jump() override;
	/**
	 * update skill logic
	 */
	virtual void UpdateSkill(float DeltaTime) override;
public:
	
	UFUNCTION(BlueprintCallable, Category = "Rust|Character")
		/// <summary>
		/// remove this character from world
		/// </summary>
		void RemoveCharacter();
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	//set ai to this character
	UFUNCTION(BlueprintCallable, Category = "Rust|Character")
		void SwitchController(ERustCharacterControllerType ControllerType, bool bForce = false);
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rust")
		FUnitState UnitState;

	/**
	 * @brief rust unit state
	 * @return FUnitState
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust")
		virtual const FUnitState& GetUnitState() const override { return UnitState; }
	/*
	 * get rust unit type
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust")
		virtual ERustUnitType GetUnitType()const override { return UnitState.UnitType; }
	UFUNCTION(BlueprintCallable, Category = "Rust")
		bool IsMovable()const{ return CanMove; }
	UFUNCTION(BlueprintCallable, Category = "Rust")
		virtual  void SetMovable(bool Movable) override;

	/**
	* get skill blueprint
	*/
	UFUNCTION(BlueprintCallable, Category = "Rust")
		virtual  USkillAsset* GetSkillAsset(int32 SkillId) override;
	/**
	* on animation state changed
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Rust")
		void OnAnimStateChanged(EGameAnimStateType State, int32 Value);

private:
	/** fixed camera view direction*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rust", meta = (AllowPrivateAccess = "true"))
	bool FixedCamera = true;
	float DefaultMeshRaw;
	UPROPERTY()
	ERustCharacterControllerType CurrentControlType;


	UPROPERTY()
		AController* AIController;
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rust", meta = (AllowPrivateAccess = "true"))
		TMap<int32, TSubclassOf<USkillAsset>> Skills;
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rust", meta = (AllowPrivateAccess = "true"))
		TMap<EGameInputType, int32> SkillKeyBindings;
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rust", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<UCameraComponent> FixedCameraCom;
	UPROPERTY()
		TMap<int32, USkillAsset*> SkillAssets;
	bool InitSkill = false;
};