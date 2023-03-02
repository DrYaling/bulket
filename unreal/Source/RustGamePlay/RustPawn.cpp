// Fill out your copyright notice in the Description page of Project Settings.


#include "RustPawn.h"

#include "AIController.h"
#include "RustGameInstance.h"

// Sets default values
ARustPawn::ARustPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CanMove = true;

}

// Called when the game starts or when spawned
void ARustPawn::BeginPlay()
{
	Super::BeginPlay();
	auto SGame = sGameInstance;
	UnitState.UnitType = ERustUnitType::Creature;
	if (SGame) {
		UnitState.Animation = SGame->GetAnimInstance(this);
	}
	UnitState.Owner = this;	
}

// Called every frame
void ARustPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARustPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ARustPawn::SetMovable(bool Movable)
{
	IRustObjectInterface::SetMovable(Movable);

	AAIController* AIController =  Cast<AAIController>(GetController());
	if(!AIController)
		return;
	if(Movable)
	{
		AIController->ResumeMove(FAIRequestID::CurrentRequest);
	}
	else
	{
		AIController->PauseMove(FAIRequestID::CurrentRequest);
	}
}
