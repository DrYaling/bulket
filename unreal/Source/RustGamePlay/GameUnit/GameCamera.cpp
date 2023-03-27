// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCamera.h"

#include "RustCharacter.h"
#include "../FFI/RustApi.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../RustGameInstance.h"
#include "Camera/CameraActor.h"

// Sets default values
AGameCamera::AGameCamera()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AGameCamera::LateInitialize()
{
	bInitialized = true;
	if (Controller)
	{
		//Controller->SetViewTargetWithBlend(nullptr, 0, VTBlend_Linear);
		AActor* Target = nullptr;
		if(const auto SGame = sGameInstance)
		{
			CurrentCharacter = SGame->GetSelectedCharacter();
			Target = CurrentCharacter;
		}
		SetLookAtTarget(Target);
	}
	else
	{
		UE_LOG(LogNative, Error, TEXT("Controller not found for player %d"), PlayerIndex);
	}
	if(const auto SGame = sGameInstance)
	{
		UE_LOG(LogNative, Display, TEXT("Register Event Delegate for player %d"), PlayerIndex);
		DelegateHandle = SGame->GameCameraEvent.AddUObject(this, &AGameCamera::OnGameCameraEvent);
	}
}

// Called when the game starts or when spawned
void AGameCamera::BeginPlay()
{
	Super::BeginPlay();
	Controller = UGameplayStatics::GetPlayerController(GetWorld(), PlayerIndex);
	if (Controller)
	{
		FName mode = FName(TEXT("Fixed"));
		Controller->SetCameraMode(mode);
		//override
		if (PlayerIndex != GetAutoActivatePlayerIndex())
		{
			GetWorld()->RegisterAutoActivateCamera(this, PlayerIndex);
			Controller->SetViewTarget(this);
		}
	}
}

void AGameCamera::Destroyed()
{
	Super::Destroyed();
	UE_LOG(LogNative, Display, TEXT("GameCamera Destroyed for player %d"), PlayerIndex);
	if (const auto SGame = sGameInstance)
	{
		if(DelegateHandle.IsValid())
			SGame->GameCameraEvent.Remove(DelegateHandle);
		UE_LOG(LogNative, Display, TEXT("UnRegister Event Delegate for player %d"), PlayerIndex);
	}
}

// Called every frame
void AGameCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!bInitialized)
	{
		LateInitialize();
	}
	//follow target
	if(CurrentCharacter)
	{
		SetActorLocation(CurrentCharacter->GetActorLocation());
	}
}

void AGameCamera::OnGameCameraEvent_Implementation(EGameCameraEvent CameraEvent, AActor* EventOwner, float Parameter)
{
	if(EventOwner)
		UE_LOG(LogNative, Display, TEXT("OnGameCameraEvent %d, %s, %f"), CameraEvent, *EventOwner->GetActorNameOrLabel(), Parameter);
	switch (CameraEvent)
	{
		case EGameCameraEvent::SelectCharacter:
		{
			if(const auto OwnerCharacter = Cast<ARustCharacter>(EventOwner))
			{
				if(OwnerCharacter == CurrentCharacter)
				{
					return;
				}
				CurrentCharacter = OwnerCharacter;
				SetLookAtTargetWithBlend(EventOwner, CharacterSwitchBlendTime);
			}
			break;
		}
		case EGameCameraEvent::BlendTarget:
		{
			SetLookAtTargetWithBlend(EventOwner, Parameter);
			break;
		}
		case EGameCameraEvent::SetViewTarget:
		{
			SetLookAtTarget(EventOwner);
			break;
		}
		case EGameCameraEvent::ScrollWheel:
		{
			const auto Camera = GetCameraComponent();
			const float FOV = Camera->FieldOfView;
			Camera->FieldOfView = FMath::Clamp(Parameter * 2.0f + FOV, 60.0, 105.0);
		}
		default:
			break;
	}
}

void AGameCamera::SetLookAtTargetWithBlend(AActor* NewViewTarget, float BlendTime, EViewTargetBlendFunction BlendFunc,
                                         float BlendExp, bool bLockOutgoing)
{
	if(Controller)
	{
		//Controller->SetViewTargetWithBlend(NewViewTarget, BlendTime, BlendFunc, BlendExp, bLockOutgoing);
		UE_LOG(LogNative, Display, TEXT("SetViewTargetWithBlend for player %d %p"), PlayerIndex, NewViewTarget);
	}
}

void AGameCamera::SetLookAtTarget(AActor* NewViewTarget)
{
	if (Controller)
	{
		//Controller->SetLookAtTarget(NewViewTarget);
		UE_LOG(LogNative, Display, TEXT("SetLookAtTarget for player %d %p"), PlayerIndex, NewViewTarget);
	}
}

