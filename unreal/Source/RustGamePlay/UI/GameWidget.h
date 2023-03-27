// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../FFI/FFITypes.h"
#include "GameWidget.generated.h"
/**
 * 
 */
UCLASS(Abstract, editinlinenew, BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True", DisableNativeTick))
class PROJECTU_API UGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	static FName UIMainName();
public:
	/**
	* for each event type, sub event is the type of this event
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Game UI Event")
		void OnGameEvent(EGameNotifyEventType EventType, int32 SubEvent, int64 EventValue, int64 EventValueExt);
	UFUNCTION(BlueprintCallable, Category = "Game UI Notify")
		void OnButtonClicked(EGameInputType Input);
	UPROPERTY(BlueprintReadWrite, DisplayName = "Widget Name")
	FName UIName;
};
