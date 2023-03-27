// Fill out your copyright notice in the Description page of Project Settings.


#include "RustActor.h"
#include "../FFI/RustPlugin.h"
#include "Utils.h"
#include "../RustGameInstance.h"


// Sets default values
ARustActor::ARustActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RustComponent = CreateDefaultSubobject<URustComponent>(TEXT("RustComponent"));
	CanMove = false;
}

// Called when the game starts or when spawned
void ARustActor::BeginPlay()
{
	Super::BeginPlay();
	auto SGame = sGameInstance;
	UnitState.UnitType = ERustUnitType::WorldObject;
	if (SGame) {
		UnitState.Animation = SGame->GetAnimInstance(this);		
	}
	UnitState.Owner = this;
}

// Called every frame
void ARustActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}