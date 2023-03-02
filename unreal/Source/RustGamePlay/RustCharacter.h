// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "RustActor.h"
#include "RustApi.h"
#include "RustProperty.h"
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
	ARustCharacter();
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
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	virtual void UnPossessed() override;
	virtual void Jump() override;
public:

	UFUNCTION(BlueprintCallable, Category = "Rust|Character")
		/// <summary>
		/// set command
		/// </summary>
		void SetCommand(EGameInputType cmd) { Command = cmd; }
	UFUNCTION(BlueprintCallable, Category = "Rust|Character")
		/// <summary>
		/// get command
		/// </summary>
		EGameInputType GetCommand() const { return Command; }
	UFUNCTION(BlueprintCallable, Category = "Rust|Character")
		/// <summary>
		/// get command
		/// </summary>
	EGameInputType ConsumeCommand(){ 
		auto cmd = Command;
		Command = EGameInputType::None;
		return cmd; 
	}
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

private:
	FVector lastCloneLocation;
	UPROPERTY()
	ERustCharacterControllerType CurrentControlType;


	UPROPERTY()
		AController* AIController;
	EGameInputType Command;
};