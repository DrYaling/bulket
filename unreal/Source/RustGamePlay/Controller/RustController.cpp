// Fill out your copyright notice in the Description page of Project Settings.


#include "RustController.h"
#include "../GameUnit/RustCharacter.h"
#include "GameFramework/Actor.h"

ARustCharacter* ARustController::GetRustCharacter() const
{
	return Cast<ARustCharacter>(GetPawn());
}

FString ARustController::GetOwnerName() const
{
	if (const auto OwnerPawn = GetPawn())
	{
		if (const auto Char = Cast<ARustCharacter>(OwnerPawn))
		{
			return Char->GetName() + "_RustCharacter";
		}
		return OwnerPawn->GetName() + "_Pawn";
	}
	if (const auto OwnerActor = GetOwner())
		return OwnerActor->GetActorNameOrLabel() + "_Actor";
	return TEXT("Empty");
}

void ARustController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ARustController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}
