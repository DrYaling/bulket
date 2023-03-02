// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RustApi.h"
#include "UObject/Object.h"
#include "RustProperty.h"
#include "RustComponent.generated.h"

//struct FDynamicRustComponent;
USTRUCT(BlueprintType)
struct FEntity
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category=Rust)
	uint64 Id;

	Entity ToRustEntity() const
	{
		Entity E;
		E.id = Id;
		return E;
	}
};

UCLASS(BlueprintType)
class UUuid : public UObject
{
	GENERATED_BODY()
public:
	Uuid Id;
};


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class PROJECTU_API URustComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URustComponent();
	FEntity Id;
	// This apprently needs to be "EditAnywhere" so that we can read and write the properties in the details panel
	// We also hide this property manually. It should only be edited through the custom Rust components panel.
	UPROPERTY(EditAnywhere, Category="Rust")
	TMap<FString, FDynamicRustComponent> Components;

public:
	UFUNCTION(BlueprintCallable, Category="Rust|Utilities", meta=(Keywords = "entity"))
	virtual FEntity GetEntity();
};
