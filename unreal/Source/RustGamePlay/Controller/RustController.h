// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RustController.generated.h"
class ARustCharacter;
/**
 * 
 */
UCLASS()
class PROJECTU_API ARustController : public APlayerController
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Rust")
	/// <summary>
	/// get rust character this controller is binding
	/// </summary>
	/// <returns>RustCharacter if valid</returns>
	FORCEINLINE	ARustCharacter* GetRustCharacter() const;
	UFUNCTION(BlueprintCallable, Category = "Rust")
		FString GetOwnerName() const;
	virtual void OnUnPossess() override;
	virtual void OnPossess(APawn* InPawn) override;
};
