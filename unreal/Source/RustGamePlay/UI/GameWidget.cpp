// Fill out your copyright notice in the Description page of Project Settings.


#include "GameWidget.h"
#include "../RustGameInstance.h"
static const FName UIMainNameDefault = FName(TEXT("UIMain"));
FName UGameWidget::UIMainName() {
	return UIMainNameDefault;
}
bool UGameWidget::Initialize()
{
	if (!Super::Initialize())
		return false;
	if (UIName == NAME_None) {
		//UE_LOG(LOgNative, Error, TEXT("UI %"))
		//return false;
		UIName = FName(GetName());
	}
	if (const auto SGame = sGameInstance) {
		SGame->SetGameUIWidget(this, UIName);
	}
	return true;
}

void UGameWidget::OnButtonClicked(EGameInputType Input)
{
	if (const auto SGame = sGameInstance) {
		SGame->OnGameInputFromInterface(Input, ETriggerEvent::Triggered, 0);
	}
}
