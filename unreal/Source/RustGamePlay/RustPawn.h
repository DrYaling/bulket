// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RustActor.h"
#include "GameFramework/Pawn.h"
#include "RustProperty.h"
#include "RustPawn.generated.h"

UCLASS()
class PROJECTU_API ARustPawn : public APawn, public IRustObjectInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARustPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual FUnitState& GetUnitState() { return UnitState; }
public:
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
		bool IsMovable()const { return CanMove; }
	UFUNCTION(BlueprintCallable, Category = "Rust")
		virtual void SetMovable(bool Movable) override;
};
