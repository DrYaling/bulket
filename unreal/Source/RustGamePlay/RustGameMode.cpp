// Fill out your copyright notice in the Description page of Project Settings.


#include "RustGameMode.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "EnhancedInputComponent.h"
#include "FFI/RustPlugin.h"
#include "FFI/RustApi.h"
#include "GameUnit/RustCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <IDirectoryWatcher.h>
#include <DirectoryWatcherModule.h>
#include "RustGameInstance.h"
#include "GameUnit/RustActor.h"
#include "GameUnit/RustPawn.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "GameConfig.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameUnit/RustActor.h"
ARustGameMode::ARustGameMode()
{
	//set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	bBlockInput = false;
}

void ARustGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ARustGameMode::StartPlay()
{
	const auto GameInstance = sGameInstance;
	if (!GameInstance)
		return;
	GameInstance->SetGameMode(this);
	const auto Plugin = GameInstance->GetPlugin();
	if (nullptr == Plugin)
	{
		UE_LOG(LogNative, Error, TEXT("PluginNative Not Initialized"));
		return;
	}
	UE_LOG(LogNative, Error, TEXT("Cha %llu, Actor Size %llu, Movement %llu, Pawn %llu"), sizeof(ACharacter), sizeof(AActor), sizeof(UCharacterMovementComponent), sizeof(APawn));
	Super::StartPlay();
	GetWorld()->AddOnActorSpawnedHandler(
		FOnActorSpawned::FDelegate::CreateUObject(this, &ARustGameMode::OnActorSpawnedHandler));


	if (!Plugin->IsLoaded() && !Plugin->TryLoad()) {
		UE_LOG(LogNative, Error, TEXT("PluginNative Load Fail"));
		return;
	}
	if (Plugin->Rust.begin_play() == ResultCode::Panic)
	{
		UE_LOG(LogNative, Error, TEXT("PluginNative begin_play Fail"));
	}
	else
	{
		Plugin->NeedsInit = false;
		UE_LOG(LogNative, Display, TEXT("PluginNative begin_play Success"));
	}
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;
		Actor->OnDestroyed.AddUniqueDynamic(this, &ARustGameMode::OnActorDestroyed);
		OnActorSpawnedHandler(Actor);
	}
}
void ARustGameMode::OnActorSpawnedHandler(AActor* actor)
{
	const auto SGame = sGameInstance;
	if (!SGame)
		return;
	constexpr EventType Type = EventType::ActorSpawned;
	ActorSpawnedEvent Event;
	Event.unitState = nullptr;
	Event.actor = static_cast<AActorOpaque*>(actor);
	if(const auto RustObject = Cast<IRustObjectInterface>(actor))
	{
		SGame->OnGameObjectSpawned(RustObject);
		auto UnitState = RustObject->GetUnitState();
		Event.unitState = &UnitState;
		const auto Plugin = SGame->GetPlugin();
		Plugin->Rust.unreal_event(&Type, static_cast<void*>(&Event));
	}
}
//
void ARustGameMode::Tick(float Dt)
{
	Super::Tick(Dt);
	//UE_LOG(LogNative, Display, TEXT("Tick %f"), Dt);
	const auto SGame = sGameInstance;
	if (!SGame)
		return;
	const auto Plugin = SGame->GetPlugin();
	if (Plugin->NeedsInit)
	{
		StartPlay();
	}
	//int32 nano = Dt * 1000000;
	if (Plugin->IsLoaded() && Plugin->Rust.tick(Dt) == ResultCode::Panic)
	{
		Plugin->Unload();
	}
}

void ARustGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	const auto SGame = sGameInstance;
	if (!SGame)
		return;
	const auto Plugin = SGame->GetPlugin();
	if (Plugin->IsLoaded() && Plugin->Rust.end_play(static_cast<REndPlayReason>(EndPlayReason)) == ResultCode::Panic)
	{
		Plugin->Unload();
	}
}

void ARustGameMode::OnReplicationPausedChanged(bool bIsReplicationPaused)
{
	Super::OnReplicationPausedChanged(bIsReplicationPaused);
}

void ARustGameMode::BeginDestroy()
{
	Super::BeginDestroy();
}

void ARustGameMode::OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	const auto SGame = sGameInstance;
	if (!SGame)
		return;
	constexpr EventType Type = EventType::ActorBeginOverlap;
	ActorBeginOverlap Event;
	Event.overlapped_actor = static_cast<AActorOpaque*>(OverlappedActor);
	Event.other = static_cast<AActorOpaque*>(OtherActor);
	const auto Plugin = SGame->GetPlugin();
	Plugin->Rust.unreal_event(&Type, static_cast<void*>(&Event));
}

void ARustGameMode::OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	const auto SGame = sGameInstance;
	if (!SGame)
		return;
	constexpr EventType Type = EventType::ActorEndOverlap;
	ActorEndOverlap Event;
	Event.overlapped_actor = static_cast<AActorOpaque*>(OverlappedActor);
	Event.other = static_cast<AActorOpaque*>(OtherActor);
	const auto Plugin = SGame->GetPlugin();
	Plugin->Rust.unreal_event(&Type, static_cast<void*>(&Event));
}

void ARustGameMode::OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	const auto SGame = sGameInstance;
	if (!SGame || !SGame->GetPlugin())
		return;
	constexpr EventType Type = EventType::ActorOnHit;
	ActorHitEvent Event;
	Event.self_actor = static_cast<AActorOpaque*>(SelfActor);
	Event.other = static_cast<AActorOpaque*>(OtherActor);
	Event.normal_impulse = ToVector3(NormalImpulse);
	const auto Plugin = SGame->GetPlugin();
	Plugin->Rust.unreal_event(&Type, static_cast<void*>(&Event));
}

void ARustGameMode::OnActorDestroyed(AActor* Actor)
{
	const auto SGame = sGameInstance;
	if (!SGame || !Actor)
		return;
	constexpr EventType Type = EventType::ActorDestroy;
	ActorDestroyEvent Event;
	Event.actor = static_cast<AActorOpaque*>(Actor);
	if (const auto RustObject = Cast<IRustObjectInterface>(Actor))
	{
		auto UnitState = RustObject->GetUnitState();
		Event.unitState = &UnitState;
		const auto Plugin = SGame->GetPlugin();
		Plugin->Rust.unreal_event(&Type, static_cast<void*>(&Event));
		SGame->OnGameObjectDestroyed(RustObject);
	}
}

void ARustGameMode::ShowUIByName(FName Widget, int32 ZOrder)
{
	if (const auto SGame = sGameInstance) {
		if (const auto Config = SGame->GetGameConfig()) {
			if (const auto WidgetBP = Config->GetGameWidget(Widget)) {
				ShowUI(WidgetBP, ZOrder);
			}
		}
	}
	else{

		UE_LOG(LogNative, Warning, TEXT("UI %s Create fail "), *Widget.ToString());
	}
}

void ARustGameMode::ShowUI(TSubclassOf<class UGameWidget> Widget, int32 ZOrder)
{
	 auto GameWidget = CreateWidget<UGameWidget>(GetWorld(), Widget);
	 if (GameWidget) {
		 if (UGameViewportSubsystem* Subsystem = UGameViewportSubsystem::Get(GetWorld())) {
			 FGameViewportWidgetSlot ViewportSlot;
			 if (false)
			 {
				 ViewportSlot = Subsystem->GetWidgetSlot(GameWidget);
			 }
			 ViewportSlot.ZOrder = ZOrder;
			 Subsystem->AddWidget(GameWidget, ViewportSlot);
		 }
		 else {
			 GameWidget->AddToViewport(ZOrder);
			 UE_LOG(LogNative, Warning, TEXT("UI sub system not found"));
		 }
	 }
	 else{

		 UE_LOG(LogNative, Warning, TEXT("UI %s Create fail "), *Widget->GetConfigName());
	 }
}
