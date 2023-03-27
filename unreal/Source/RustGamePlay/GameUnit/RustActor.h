// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Components/RustComponent.h"
#include "../Components/RustProperty.h"
#include "GameFramework/Actor.h"
#include "RustActor.generated.h"
UINTERFACE(MinimalAPI)
class URustObjectInterface: public UInterface
{
	GENERATED_BODY()
};

class PROJECTU_API IRustObjectInterface
{
	GENERATED_BODY()
public:
	virtual const FUnitState& GetUnitState() const { return FUnitState::Default(); }
	virtual FUnitState& GetUnitState() = 0;
	virtual ERustUnitType GetUnitType() const { return GetUnitState().UnitType; }
	FORCEINLINE class URustAnimInstance* GetAnimInstance() const { return GetUnitState().Animation; }
	/**
	 * update skills
	 */
	virtual void UpdateSkill(float DeltaTime){}
	//get skill asset this unit is taken
	virtual class USkillAsset* GetSkillAsset(int32 SkilId) {
		return nullptr;
	}
	/**
	 * @brief set unit movable
	 * @param Movable 
	 */
	virtual void SetMovable(bool Movable) { CanMove = Movable; }
	/**
	 * @brief get move state
	 * @return moveable
	 */
	bool GetMovable() const { return CanMove; }
protected:
	uint8 CanMove: 1;
};
UCLASS()
class ARustActor : public AActor, public IRustObjectInterface
{
	GENERATED_BODY()

public:
	uint64 Entity;
	// Sets default values for this actor's properties
	ARustActor();
	UPROPERTY(Category=Rust, EditAnywhere)
	TObjectPtr<URustComponent> RustComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rust")
		FUnitState UnitState;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category="Rust", meta=(DisplayName="Get Entity Component"))
	URustComponent* GetRustComponent() { return RustComponent; }
	/**
	 * @brief rust unit state
	 * @return FUnitState
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust")
		virtual const FUnitState& GetUnitState() const override { return UnitState; }
	virtual FUnitState& GetUnitState() override { return UnitState; }
	/*
	 * get rust unit type
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust")
	virtual ERustUnitType GetUnitType()const override { return GetUnitState().UnitType; }
};
