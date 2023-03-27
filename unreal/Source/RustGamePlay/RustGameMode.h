// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RustGameMode.generated.h"
class ARustCharacter;

/**
 *
 */
UCLASS()
class PROJECTU_API ARustGameMode : public AGameModeBase
{
	GENERATED_BODY()
		ARustGameMode();
	int32 Handle;
		void OnActorSpawnedHandler(AActor* actor);
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void StartPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnReplicationPausedChanged(bool bIsReplicationPaused) override;
	virtual void BeginDestroy() override;

public:

	UFUNCTION()
		void OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
		void OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
		void OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void OnActorDestroyed(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Rust|UI")
		void ShowUI(TSubclassOf<class UGameWidget> Widget, int32 ZOrder =  0);
	UFUNCTION(BlueprintCallable, Category = "Rust|UI")
		void ShowUIByName(FName Widget, int32 ZOrder = 0);
private:

};

