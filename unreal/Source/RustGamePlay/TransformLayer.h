#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Components/RustProperty.h"
#include "TransformLayer.generated.h"
class ARustCharacter;
class ARustController;
class ARustPawn;
class ARustActor;
class URustGameInstance;
class URustComponent;
/**
 * data and events Transform layer between ue and rust
 */
UCLASS()
class UTransformLayer: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// --------------- Spawn ---------------
	/**
	 *spawn rust object by object bp name
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
	static AActor* SpawnObjectByName(const FString& BPName, const FTransform& Transform, FUnitState UnitState, AActor* Owner = nullptr);
	/**
	 * spawn rust object by blueprint class
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
	static AActor* SpawnObjectByBlueprint(TSubclassOf<UClass> character, const FTransform& Transform, FUnitState UnitState, AActor* Owner = nullptr);

	// -------------------- Network ---------------------------
	/**
	 * get network delay in ms
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		static int32 GetNetworkDelay();
	/**
	 * check if this client is also server
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		static bool IsRunAsServer();

	// -------------------- GamePlay -------------------------

	/**
	 * get game instance
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		static URustGameInstance* GetGameInstance();
	/**
	 * get rust object state if actor is rust object
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		static const FUnitState& GetRustObjectState(const AActor* Actor);
	/**
	 * get rust object state if actor is rust object
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		static ERustUnitType GetRustObjectType(const AActor* Actor); 
	/**
	* get unit type by uuid
	*/
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		static ERustUnitType GetUnitType(int32 UUID);


	// ----------------------- Animation ------------------------

	/**
	 * get a animation name hash
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		static int32 GetNameHash(const FString& Name);
	UFUNCTION(BlueprintCallable, Category = "Rust|Animation")
		static UAnimSequence* AnimationClip(class UAnimSequence* Anim, int StartFrame, int EndFrame);


};

