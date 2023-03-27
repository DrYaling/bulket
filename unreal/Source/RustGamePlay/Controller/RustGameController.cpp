// Fill out your copyright notice in the Description page of Project Settings.


#include "RustGameController.h"
#include "../FFI/RustApi.h"
#include "../RustGameInstance.h"
#include "RustController.h"
#include "Engine/LocalPlayer.h"
#include <EnhancedInputActionDelegateBinding.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>

// Sets default values
ARustGameController::ARustGameController()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bBlockInput = false;
	/*InputComponent = CreateDefaultSubobject<UEnhancedInputComponent>(TEXT("UEnhancedInputComponent"));
	InputComponent->bBlockInput = bBlockInput;
	InputComponent->Priority = InputPriority;
	UEnhancedInputActionDelegateBinding::BindInputDelegates(GetClass(), InputComponent);*/
}

// Called when the game starts or when spawned
void ARustGameController::BeginPlay()
{
	Super::BeginPlay();
	if (!InputComponent) {
		InputComponent = CreatePlayerInputComponent();
	}
	UE_LOG(LogNative, Display, TEXT("ARustGameController::BeginPlay %d"), InputEnabled());
	if (Controller == nullptr || CastChecked<ARustController>(Controller) == nullptr)
	{
		UE_LOG(LogNative, Display, TEXT("ARustGameController::Reload Controller %p"), Controller);
		Controller = GetWorld()->SpawnActor<ARustController>(DefaultController, FTransform::Identity);
		Controller->Possess(this);
	}
	if (!sGameInstance)
		return;
#if WITH_EDITOR
	UE_LOG(LogNative, Display, TEXT("GameController Spawned"));
#endif
	if(const int32 PlayerCount = sGameInstance->GetNumLocalPlayers() > 0)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(sGameInstance->GetLocalPlayerByIndex(0)))
		{
			UE_LOG(LogNative, Display, TEXT("ARustGameController players count %d"), PlayerCount);
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	else {
		UE_LOG(LogNative, Warning, TEXT("ARustGameController players count %d"), PlayerCount);
	}
}

void ARustGameController::BeginDestroy() {
	Super::BeginDestroy();
#if WITH_EDITOR
	UE_LOG(LogNative, Display, TEXT("GameController Destroyed"));
#endif
}

// Called every frame
void ARustGameController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARustGameController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	const auto GameInstance = sGameInstance;
	if (!GameInstance)
		return;
	UE_LOG(LogNative, Display, TEXT("ARustGameController::SetPlayerInputComponent"));
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		if (const auto SGame = sGameInstance)
			SGame->OnInputChanged(EnhancedInputComponent);
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, GameInstance, &URustGameInstance::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, GameInstance, &URustGameInstance::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, GameInstance, &URustGameInstance::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, GameInstance, &URustGameInstance::Look);
		
	}

}

