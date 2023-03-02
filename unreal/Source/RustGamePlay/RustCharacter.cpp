// Fill out your copyright notice in the Description page of Project Settings.


#include "RustCharacter.h"

#include "AIController.h"
#include "RustController.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "RustGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "RustApi.h"
#include "RustPlugin.h"
#include "RustGameInstance.h"
// Sets default values
ARustCharacter::ARustCharacter()
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
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	UGameplayStatics::SetViewportMouseCaptureMode(GetWorld(), EMouseCaptureMode::CaptureDuringRightMouseDown);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	UE_LOG(LogNative, Warning, TEXT("Actor constructed %s"), *GetName());
	CanMove = true;
	InitAsNewSpawned();

}
void ARustCharacter::InitAsNewSpawned() {
	lastCloneLocation = GetActorLocation();
	CurrentControlType = ERustCharacterControllerType::None;
}

// Called when the game starts or when spawned
void ARustCharacter::BeginPlay()
{
	Super::BeginPlay();

	//save controller here because constructor will not be called in blueprint 
	lastCloneLocation = GetActorLocation();
	UnitState.UnitType = ERustUnitType::Character;
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
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARustCharacter::Look);

	}

}


void ARustCharacter::Move(const FInputActionValue& Value)
{
	if(!CanMove)
		return;
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FVector location = GetActorLocation();
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		// find out which way is forward
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);

		AddMovementInput(RightDirection, MovementVector.X);

		//test ray cast
		const FVector forward = GetActorForwardVector();
		FHitResult result;
		TArray<AActor*> ignores;
		if (UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			location, forward * 1000 + location, TraceTypeQuery1, false,
			ignores, EDrawDebugTrace::Type::ForOneFrame, result,
			true))
		{
			const auto target = result.GetHitObjectHandle().GetManagingActor();
			ARustCharacter* targetPlayer = dynamic_cast<ARustCharacter*>(target);
			if (targetPlayer != nullptr)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Hit Character %s"), *targetPlayer->GetName());
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("Hit Actor %s"), *target->GetName());
				auto const Location = GetActorLocation();
				if (FVector::Distance(Location, lastCloneLocation) > 2.0)
				{
					////auto gameCharacter = GetWorld()->GetGameState()->GetDefaultGameMode()->DefaultPawnClass;
					//auto gameCharacter = StaticLoadObject(StaticClass(), nullptr, TEXT("Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
					//FActorSpawnParameters Parameters;
					//Parameters.Name = TEXT("Partner") + partners.Num() + 1;
					//Parameters.Owner = Owner;
					//Parameters.Template = Cast<AActor>(gameCharacter);
					//Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					//AActor* aPtr = GetWorld()->SpawnActor<AActor>(gameCharacter->StaticClass(), Location, Rotation, Parameters);
					//partners.Add(Cast<ACharacter>(aPtr));
					/*UObject* Outer = (UObject*)GetTransientPackage();
					AGTACharacter* aPtr = NewObject<AGTACharacter>(Outer, StaticClass(), TEXT("Patner"), RF_NoFlags, this);
					partners.Add(aPtr);*/
					lastCloneLocation = Location;
				}

			}
		}
	}
}

void ARustCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
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
