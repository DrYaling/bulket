// Fill out your copyright notice in the Description page of Project Settings.

#include "RustGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "RustApi.h"
#include "RustPlugin.h"
#include "RustGameController.h"
#include "RustCharacter.h"
#include "RustGameMode.h"
#include "EngineUtils.h"
#include "RustController.h"
#include "GameFramework/PlayerStart.h"
#include <EnhancedInputComponent.h>
#include "GameConfig.h"
#include "RustPawn.h"

static URustGameInstance* instance = nullptr;
URustGameInstance* URustGameInstance::GetInstance() {
	return instance;
}
void URustGameInstance::OnCharacterSwitchToAI(ARustCharacter* Character) {
	if (SelectedCharacter == Character) {

	}
}
void URustGameInstance::OnInputChanged(UEnhancedInputComponent* EnhancedInputComponent) {
	if (ControllAction) {
		//ControllAction
		EnhancedInputComponent->BindAction(ControllAction, ETriggerEvent::Started, this, &URustGameInstance::InputControll);
		EnhancedInputComponent->BindAction(ControllAction, ETriggerEvent::Completed, this, &URustGameInstance::InputControll);
	}
	else {
		UE_LOG(LogNative, Warning, TEXT("ControllAction is null"));
	}
}
void URustGameInstance::OnGameObjectSpawned(IRustObjectInterface* Object)
{
	AActor* Actor = Cast<AActor>(Object);
	if (!Actor)
		return;
	const auto UUID = GetUnitUUID();
	auto& UnitState = Object->GetUnitState();
	UnitState.Uuid = UUID;
	UE_LOG(LogNative, Display, TEXT("Object %d of %d spawned"), UUID, UnitState.UnitType);
	TScriptInterface<IRustObjectInterface> RefObject;
	RefObject.SetObject(Actor);
	RefObject.SetInterface(Object);
	AllObjects.Add(UUID, RefObject);
}
void URustGameInstance::OnGameObjectDestroyed(IRustObjectInterface* Object)
{
	const auto UUID = Object->GetUnitState().Uuid;
	AllObjects.Remove(UUID);
}

IRustObjectInterface* URustGameInstance::GetObjectByUUID(int32 UUID)
{
	const auto Target = AllObjects.Find(UUID);
	return Target != nullptr ? Target->GetInterface() : nullptr;
}

ARustCharacter* URustGameInstance::SpawnCharacter(FString BPName, FTransform transform, FString name)
{
	UE_LOG(LogNative, Display, TEXT("Spawn Character %s"), *BPName);
	if (!GameConfig)
		return nullptr;
	auto CharacterTemplate = GameConfig->GetCharacterTemplate(FName(BPName));
	auto cha = CharacterTemplate != nullptr ?
		GetWorld()->SpawnActor<ARustCharacter>(*CharacterTemplate, transform) :
		nullptr;
	if (cha)
	{
		if(!name.IsEmpty())
			cha->SetActorLabel(name);
		cha->InitAsNewSpawned();
		cha->SwitchController(ERustCharacterControllerType::Ai, true);
	}
	else {
		UE_LOG(LogNative, Error, TEXT("CharacterMapping was empty, or %s not match"), *BPName);
	}
	return cha;
}
ARustPawn* URustGameInstance::SpawnCreature(FString BPName, FTransform transform, FString name)
{
	return nullptr;
}

ARustPawn* URustGameInstance::SpawnWorldObject(FString BPName, FTransform transform, FString name)
{
	return nullptr;
}
URustAnimInstance* _GetAnimInstance(AActor* Current) {
	auto Mesh = Current->FindComponentByClass<USkeletalMeshComponent>();
	if (Mesh) {
		return Cast<URustAnimInstance>(Mesh->AnimClass);
	}
	return nullptr;
}
URustAnimInstance* URustGameInstance::GetAnimInstance(IRustObjectInterface* Owner)
{
	AActor* Actor = Cast<AActor>(Owner);
	if (Actor) {
		TArray<USkeletalMeshComponent*> Array;
		Actor->GetComponents(Array);
		for (int32 i = 0; i < Array.Num(); ++i) {
			USkeletalMeshComponent* Mesh = Array[i];
			if (Mesh) {
				UE_LOG(LogNative, Display, TEXT("GetAniIns mesh %p, %p"), Mesh, Mesh->AnimClass);
				return Cast<URustAnimInstance>(Mesh->GetAnimInstance());
			}
		}
	}
	return nullptr;
}

IRustObjectInterface* URustGameInstance::GetGameObject(int32 UUID)
{
	if (const auto GameObject = AllObjects.Find(UUID))
		return GameObject->GetInterface();
	return nullptr;
}

void URustGameInstance::InputControll(const FInputActionInstance& Value)
{
	const float Min = 0.45;
	const float InputValue = Value.GetValue().Get<float>();
	if (InputValue < Min)
		return;
	for (const auto& kv: KeyBindings)
	{
		const float value = kv.Value;
		if (std::abs(value - InputValue) < Min) {
			RustPlugin->Rust.on_game_input(kv.Key, Value.GetTriggerEvent(), Value.GetElapsedTime());
			if (kv.Key == EGameInputType::ControlA) {
				/*if (true) {
					APlayerStart* pStart = nullptr;

					UWorld* World = GetWorld();
					for (TActorIterator<APlayerStart> It(World); It; ++It)
					{
						pStart = *It;
						break;
					}
					if (nullptr == pStart) {
						UE_LOG(LogNative, Error, TEXT("PlayerStart was empty, spawn fail"));
					}
					else {
						auto Character
						auto cha = CharacterMapping.Contains(DefaultCharacter) ? 
							GetWorld()->SpawnActor<ARustCharacter>(CharacterMapping[DefaultCharacter], pStart->GetActorTransform()) : 
							nullptr;
						if (cha)
						{
							cha->InitAsNewSpawned();
							SelectCharacter(cha);
						}
						else {
							UE_LOG(LogNative, Error, TEXT("CharacterMapping was empty, or DefaultCharacter not match"));
						}
					}
				}*/
			}
			else if (kv.Key == EGameInputType::ControlB) {
				if (GameCharacters.Num() >= 1)
					SelectCharacter(GameCharacters[0]);
			}
			else if (kv.Key == EGameInputType::ControlC) {
				if (GameCharacters.Num() >= 2)
					SelectCharacter(GameCharacters[1]);
			}
			else if (kv.Key == EGameInputType::ControlD) {
				if (GameCharacters.Num() >= 3)
					SelectCharacter(GameCharacters[2]);
			}
			else if (kv.Key == EGameInputType::SkillBar1) {
				if (SelectedCharacter) {
					//SelectedCharacter->SetCommand(kv.Key);
				/*	if (auto Montage = GameConfig->GetMontage("Attack")) {
						if(!SelectedCharacter->GetCurrentMontage())
						{
							float len = SelectedCharacter->PlayAnimMontage(Montage, 1.0, "BaseAtk");
							UE_LOG(LogNative, Display, TEXT("Play attack %p with len %f"), Montage, len);
						}
					}*/
				}
			}
			return;
		}
	}
}
int32 URustGameInstance::GetUnitUUID()
{
	return ++UnitUUID;
}
void URustGameInstance::Move(const FInputActionValue& Value)
{
}
void URustGameInstance::Look(const FInputActionValue& Value)
{
}
void URustGameInstance::Jump()
{
}
void URustGameInstance::StopJumping()
{
}
void URustGameInstance::Init()
{
	SelectedCharacter = nullptr;
}
void URustGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
	UE_LOG(LogNative, Display, TEXT("StartGameInstance"));
}

void URustGameInstance::LoadComplete(const float LoadTime, const FString& MapName)
{
	Super::LoadComplete(LoadTime, MapName);
	UE_LOG(LogNative, Display, TEXT("LoadComplete %f %s"), LoadTime, *MapName);
}

void URustGameInstance::OnStart()
{
	instance = this;
	GameConfig = NewObject<UGameConfig>(this, GameConfigClass, TEXT("GameConfig"));
	RustPlugin = new FPlugin();
	const auto loadResult = RustPlugin->TryLoad();
	Super::OnStart();
	UE_LOG(LogNative, Display, TEXT("Size of uObject %d"), sizeof(UObject));
	if(!loadResult)
		UE_LOG(LogNative, Error, TEXT("RustPlugin Load Fail"));
}

void URustGameInstance::Shutdown()
{
	Super::Shutdown();
	instance = nullptr;
	delete RustPlugin;
	RustPlugin = nullptr;	
	UE_LOG(LogNative, Display, TEXT("Shutdown"));
}

void URustGameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	//UE_LOG(LogNative, Display, TEXT("OnWorldChanged from %s to %s"), *OldWorld->GetName(), *NewWorld->GetName());
}

ARustCharacter* URustGameInstance::GetCharacter(FString name) const
{
	for (const auto cha : GameCharacters)
	{
		//TODO
		if (cha->GetName() == name)
		{
			return cha;
		}
	}
	return nullptr;
}

ARustCharacter* URustGameInstance::GetCharacterByIndex(int32 index) const
{
	if (index >= 0 && index < GameCharacters.Num())
		return GameCharacters[index];
	return nullptr;
}

void URustGameInstance::OnCharacterSpawn(ARustCharacter* Character)
{
	GameCharacters.Add(Character);
	if (GameCharacters.Num() == 1)
		SelectCharacter(Character);
}

void URustGameInstance::OnCharacterDestroyed(ARustCharacter* Character)
{
	GameCharacters.Remove(Character);
	if (SelectedCharacter == Character)
	{
		SelectedCharacter = nullptr;
		if (GameCharacters.Num() != 0)
		{
			SelectCharacter(GameCharacters[0]);
		}
	}
}

void URustGameInstance::SelectCharacter(ARustCharacter* Character)
{
	const auto Old = SelectedCharacter;
	SelectedCharacter = Character;
	if (Old != nullptr)
		Old->SwitchController(ERustCharacterControllerType::Ai);
	if (SelectedCharacter != nullptr)
		SelectedCharacter->SwitchController(ERustCharacterControllerType::Player, true);
}
