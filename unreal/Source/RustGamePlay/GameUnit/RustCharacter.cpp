// Fill out your copyright notice in the Description page of Project Settings.


#include "RustCharacter.h"

#include "AIController.h"
#include "../Controller/RustController.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "../RustGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "../FFI/RustApi.h"
#include "../FFI/RustPlugin.h"
#include "../RustGameInstance.h"
#include "../Components/AMC_MovementComponent.h"
// Sets default values
ARustCharacter::ARustCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAMC_MovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	UGameplayStatics::SetViewportMouseCaptureMode(GetWorld(), EMouseCaptureMode::CaptureDuringRightMouseDown);
	if (FixedCamera) {
		GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
		GetCharacterMovement()->RotationRate = FRotator(0.0f, -90.0f, 0.0f); // ...at this rotation rate
		//if (FollowCamera == nullptr) {
		//	// Create a follow camera
		//	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
		//	FollowCamera->SetupAttachment(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
		//	FTransform transform;
		//	transform.SetLocation(FVector(-44.0f, 505.0f, 130.0f));
		//	////(Pitch=-2.837983,Yaw=-115.769049,Roll=-88.028803)
		//	// (Pitch=-2.837983,Yaw=-115.769049,Roll=-88.028803)
		//	transform.SetRotation(FQuat(FRotator(-2.83f, -115.76f, -88.03f)));
		//	FollowCamera->SetRelativeTransform(transform);
		//	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
		//}
		//FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
	}
	else {
		GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
		GetCharacterMovement()->RotationRate = FRotator(0.0f, -90.0f, 0.0f); // ...at this rotation rate
		// Create a camera boom (pulls in towards the player if there is a collision)
		CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
		CameraBoom->SetupAttachment(RootComponent);
		CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
		CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
		FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
		FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
		FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	}
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	UE_LOG(LogNative, Warning, TEXT("Actor constructed %s"), *GetName());
	CanMove = true;
	InitAsNewSpawned();

}
void ARustCharacter::InitAsNewSpawned() {
	CurrentControlType = ERustCharacterControllerType::None;
}

// Called when the game starts or when spawned
void ARustCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetActorRotation(FRotator(0.0f, -90.0f, 0.0f)); // ...at this rotation rate
	UnitState.UnitType = ERustUnitType::Character;
	if (FixedCamera) {
		TArray<UCameraComponent*> Array;
		GetComponents(Array);
		if (Array.Num() > 0) {
			FixedCameraCom = Array[0];
		}
	}
	//init skills
	for (auto KV : Skills) {
		if (KV.Value) {
			FString SkillName = FString::Printf(TEXT("SkillAsset%d"), KV.Key);
			USkillAsset* SkillObject = NewObject<USkillAsset>(this, *KV.Value, FName(SkillName));
			if (SkillObject) {
				SkillAssets.Add(KV.Key, SkillObject);
			}
		}
	}
	auto SGame = sGameInstance;
	if (SGame) {
		SGame->OnCharacterSpawn(this);
		UnitState.Animation = SGame->GetAnimInstance(this);
	}
	UnitState.Owner = this;
	if (const APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ARustCharacter::Destroyed()
{
	Super::Destroyed();
	for (const auto KV : SkillAssets)
	{
		if (KV.Value)
			KV.Value->UnloadSkill();
	}
	SkillAssets.Empty(0);
	if(const auto SGame = sGameInstance)
		SGame->OnCharacterDestroyed(this);
}

void ARustCharacter::RemoveCharacter()
{
	//GetRustModule().GameMode->OnCharacterDestroyed(this);
	GetWorld()->RemoveActor(this, false);
}

// Called every frame
void ARustCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!InitSkill) {
		InitSkill = true;
		UE_LOG(LogNative, Display, TEXT("InitSkill Character"));
		//init skills
		TArray<SkillKeyBinding> Array;
		for (auto KV : SkillAssets) {
			SkillKeyBinding Binding;
			Binding.skill_id = KV.Key;
			Binding.key = EGameInputType::None;
			for (auto KeyBinding : SkillKeyBindings) {
				if (KeyBinding.Value == Binding.skill_id) {
					Binding.key = KeyBinding.Key;
					break;
				}
			}
			Array.Add(Binding);
		}
		//init character skills
		if (const auto SGame = sGameInstance)
		{
			if (const auto Plugin = SGame->GetPlugin()) {
				Plugin->Rust.set_unit_skills(GetUnitState().Uuid, Array.GetData(), Array.Num());
				for (auto KV : SkillAssets) {
					EGameInputType Key = EGameInputType::None;
					KV.Value->InitSkillAsset(this, KV.Key);
				}
			}
		}
	}
	UpdateSkill(DeltaTime);
}

// Called to bind functionality to input
void ARustCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UE_LOG(LogNative, Display, TEXT("SetPlayerInput %s"), *GetName());
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		if (const auto SGame = sGameInstance)
			SGame->OnInputChanged(EnhancedInputComponent);
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ARustCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARustCharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARustCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARustCharacter::Look);

	}

}


void ARustCharacter::Move(const FInputActionValue& Value)
{
	if(!CanMove)
		return;
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();
	//const FVector location = GetActorLocation();
	if (Controller != nullptr)
	{
		if (MovementVector.SquaredLength() < 0.001)
			return;
		FRotator Rotation = GetActorRotation();// Controller->GetControlRotation();
		//// find out which way is forward
		//const FRotator YawRotation(0, Rotation.Yaw, 0);

		//// get forward vector
		//const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//// get right vector 
		//const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement 
		AddMovementInput(FVector::ForwardVector, -MovementVector.Y);

		AddMovementInput(FVector::RightVector, -MovementVector.X);
		//turn
		if (FMath::Abs(MovementVector.X) > 0.001) {
			double Yaw = Rotation.Yaw;
			while (Yaw < -180.0)
				Yaw += 360.0;
			while (Yaw > 180.0) {
				Yaw -= 360.0;
			}
			//UE_LOG(LogNative, Display, TEXT("Try Turn Rotation %f, %f"), Yaw, MovementVector.X);
			if (MovementVector.X * Yaw > 0.0) {
				//UE_LOG(LogNative, Display, TEXT("Turn Rotation"));
				SetActorRotation(FRotator(0.0f, MovementVector.X > 0.0 ? -90.0f : 90.0f, 0.0f));
			}

		}
	}
}

void ARustCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	// 
	const float Wheal = Value.Get<float>();
	//if (Controller != nullptr)
	//{
	//	// add yaw and pitch input to controller
	//	AddControllerYawInput(LookAxisVector.X);
	//	AddControllerPitchInput(LookAxisVector.Y);
	//}
	if (const auto SGame = sGameInstance) {

		SGame->GameCameraEvent.Broadcast(EGameCameraEvent::ScrollWheel, this, Wheal);
	}
}

void ARustCharacter::UnPossessed()
{
	Super::UnPossessed();
}

void ARustCharacter::Jump()
{
	if (!CanMove)
		return;
	Super::Jump();
}

void ARustCharacter::UpdateSkill(float DeltaTime)
{
	for (const auto KV: SkillAssets)
	{
		if (KV.Value)
			KV.Value->ProcessTick(DeltaTime);
	}
}

void ARustCharacter::SwitchController(ERustCharacterControllerType ControllerType, bool bForce)
{
	UE_LOG(LogNative, Display, TEXT("%s Controller %d, %d, %d"), *GetName(), Controller, CurrentControlType, ControllerType);
	//if (!bForce && CurrentControlType == ControllerType)
	//	return;
	if(ControllerType == ERustCharacterControllerType::Ai)
	{
		if(!AIController)
		{
			//PlayerController = Controller;
			const auto transform = GetActorTransform();
			UE_LOG(LogNative, Display, TEXT("Spawn AIController"));
			AIController = GetWorld()->SpawnActor<AController>(AIControllerClass, transform);
		}
		UE_LOG(LogNative, Display, TEXT("Set AIController %p, %p"), AIController, Controller);
		if(bForce || CurrentControlType != ControllerType)
		{
			AIController->Possess(this);
			SetMovable(CanMove);
		}
	}
	else
	{
		const auto controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);		
		UE_LOG(LogNative, Display, TEXT("Set PlayerController %p, %p"), Controller, controller);
		if (bForce || CurrentControlType != ControllerType)
		{
			controller->Possess(this);
		}
		//Cast<APlayerController>(PlayerController)->SetViewTarget(this);
	}
	CurrentControlType = ControllerType;
}


void ARustCharacter::SetMovable(bool Movable)
{
	IRustObjectInterface::SetMovable(Movable);
	if(CurrentControlType == ERustCharacterControllerType::Ai)
	{
		AAIController* AC = Cast<AAIController>(AIController);
		if (!AC)
			return;
		if (Movable)
		{
			AC->ResumeMove(FAIRequestID::CurrentRequest);
		}
		else
		{
			AC->PauseMove(FAIRequestID::CurrentRequest);
		}
	}
}

USkillAsset* ARustCharacter::GetSkillAsset(int32 SkillId)
{
	if (const auto Skill = SkillAssets.Find(SkillId))
		return *Skill;
	return nullptr;
}
