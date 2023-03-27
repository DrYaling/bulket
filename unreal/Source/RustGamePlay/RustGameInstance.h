// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "UI/GameWidget.h"
#include "GameTypes.h"
#include "RustGameInstance.generated.h"
class FPlugin;
class ARustGameMode;
class ARustGameController;
class ARustCharacter;
class ARustController;
class ARustPawn;
class UGameConfig;
class ARustActor;
class IRustObjectInterface;
DECLARE_MULTICAST_DELEGATE_ThreeParams(FGameCameraEventDelegate, EGameCameraEvent, AActor*, float);
//typedef GameCameraEventDelegate::FDelegate FGameCameraEventDelegate;
/**
 * 
 */
UCLASS()
class PROJECTU_API URustGameInstance : public UGameInstance
{
	GENERATED_BODY()

		friend class ARustGameMode;
public:
	virtual void Init() override;
	virtual void StartGameInstance() override;
	virtual void LoadComplete(const float LoadTime, const FString& MapName) override;
	virtual void OnStart() override;
	virtual void Shutdown() override;
	virtual void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;
public:
	void SetGameMode(ARustGameMode* gm) { _GameMode = gm; }
	ARustGameMode* GameMode() const { return _GameMode; }
	FPlugin* GetPlugin() const { return RustPlugin; }
	static URustGameInstance* GetInstance();
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	void Jump();
	void StopJumping();
	// get player controller  instance
	void OnCharacterSpawn(ARustCharacter* Character);
	void OnCharacterSwitchToAI(ARustCharacter* Character);
	void OnCharacterDestroyed(ARustCharacter* Character);
	void OnInputChanged(class UEnhancedInputComponent*);
	/*
	* game obejct spawned
	*/
	void OnGameObjectSpawned(IRustObjectInterface* Object);
	/*
	* game object destroyed
	*/
	void OnGameObjectDestroyed(IRustObjectInterface* Object);
	/**
	 * get game object by uuid
	 */
	FORCEINLINE IRustObjectInterface* GetObjectByUUID(int32 UUID);
	/**
	* game input from ui
	*/
	void OnGameInputFromInterface(EGameInputType Input, ETriggerEvent TriggerEvent, float Duration);
	/**
	 *get game config
	 */
	UGameConfig* GetGameConfig() const
	{
		return GameConfig;
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rust")
		class UInputAction* ControllAction;
	///default character to be spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rust")
		FName DefaultCharacter;
	/// <summary>
	/// key binding to input float value
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rust")
		TMap<EGameInputType, float> KeyBindings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rust")
		bool BlockInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rust")
		TSubclassOf<ARustGameController> DefaultGameController;
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		ARustCharacter* GetCharacter(FString name) const;
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		ARustCharacter* GetCharacterByIndex(int32 index) const;
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
	/**
	 *spawn a character
	 */
	ARustCharacter* SpawnCharacter(FString BPName, FTransform transform, FString name);
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
	/**
	 *spawn a rust Creature(monster npc)
	 */
	ARustPawn* SpawnCreature(FString BPName, FTransform transform, FString name);
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
	/**
	 *spawn a rust pawn(monster npc)
	 */
	ARustPawn* SpawnWorldObject(FString BPName, FTransform transform, FString name);

	/**
	 * @brief set character selected
	 * @param Character
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust")
		void SelectCharacter(ARustCharacter* Character);
	/**
	 * @brief get current selected character
	 * @return
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust")
		ARustCharacter* GetSelectedCharacter() const { return SelectedCharacter; }
	/**
	 * check if this game is server
	 * @return
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust")
		bool IsGameServer() const { return IsServer; }
	UFUNCTION(BlueprintCallable, Category = "Rust|Game UI")
		/**
		 *set ui widget
		 */
		void SetGameUIWidget(UGameWidget* Window, FName UIName);
	UFUNCTION(BlueprintCallable, Category = "Rust|Game UI")
		/**
		 *get ui widget
		 */
		UGameWidget* GetGameUIWidget(FName UIName);
	UFUNCTION(BlueprintCallable, Category = "Rust|Game UI")
	UGameWidget* GetGameMainUI() {
		return UIMain;
	}
	/*
	* get target actor anim instance
	*/
	class URustAnimInstance* GetAnimInstance(IRustObjectInterface* Owner);
	/*
	* get get GameObject
	*/
	 IRustObjectInterface* GetGameObject(int32 UUID);
	/**
	 * @brief game event delegate
	 */
	//UPROPERTY(BlueprintReadWrite, Category = "Rust|Camera")
	FGameCameraEventDelegate GameCameraEvent;
private:
	void InputControll(const FInputActionInstance& Value);
	FORCEINLINE int32 GetUnitUUID(ERustUnitType UnitType);
private:
		FPlugin* RustPlugin;
		UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rust", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UGameConfig> GameConfigClass;
		UPROPERTY()
			UGameConfig* GameConfig;
		UPROPERTY()
		ARustGameMode* _GameMode;
	/**
	 * @brief characters in game
	 */
	UPROPERTY()
		TArray<ARustCharacter*> GameCharacters;
	/**
	 * @brief character selected this time
	 */
	UPROPERTY()
		ARustCharacter* SelectedCharacter;
	/**
	 *dynamic world game objects
	 *
	 */
	UPROPERTY()
		TMap<int32, TScriptInterface<IRustObjectInterface>> AllObjects;
	UPROPERTY()
		TMap<FName, UGameWidget*> UIWidgets;
	UPROPERTY()
		UGameWidget* UIMain;

	bool IsServer = false;
	int32 UnitUUID = 0;
};
#define sGameInstance URustGameInstance::GetInstance()