// Fill out your copyright notice in the Description page of Project Settings.


#include "RustComponent.h"




// Sets default values for this component's properties
URustComponent::URustComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FEntity URustComponent::GetEntity()
{
	return Id;
}



