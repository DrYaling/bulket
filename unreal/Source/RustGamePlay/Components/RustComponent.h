// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../FFI/RustApi.h"
#include "UObject/Object.h"
#include "RustProperty.h"
#include "RustComponent.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class PROJECTU_API URustComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URustComponent();
	virtual void InitializeComponent() override;
private:
	/**
	 * @brief rust component handler, logic updated from native code, c++ layer only deal with serializer and detail display
	 */
	void* NativeComponentHandler;
};
