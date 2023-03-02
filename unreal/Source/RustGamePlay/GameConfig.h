// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Templates/UnrealTemplate.h"
#include "GameConfig.generated.h"
class ARustPawn;
class ARustActor;
class ARustCharacter;
class UAnimMontage;
USTRUCT(BlueprintType)
struct FMontageConfig {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rust")
		FString Path;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rust")
		TSoftObjectPtr<UAnimMontage> Montage;
};
/**
 * 
 */
UCLASS(config = Game, BlueprintType, Blueprintable)
class PROJECTU_API UGameConfig : public UObject
{
	GENERATED_BODY()
	
public:
	/*
	* get montage template
	*/
	UFUNCTION(BlueprintCallable)
		UAnimMontage* GetMontage(FName Name);
	/*
	* get montage template
	*/
	UFUNCTION(BlueprintCallable)
		TSoftObjectPtr<UAnimMontage> GetMontageAsset(FName Name) const;
	/*
	* get world object template
	*/
	UFUNCTION(BlueprintCallable)
		TSubclassOf<ARustActor> GetWorldObjectTemplate(FName Name) const;
	/*
	* get creature template
	*/
	UFUNCTION(BlueprintCallable)
		TSubclassOf <ARustPawn> GetCreatureTemplate(FName Name) const;
	/*
	* get character template
	*/
	UFUNCTION(BlueprintCallable)
		TSubclassOf<ARustCharacter> GetCharacterTemplate(FName Name) const;
private:
	/*
	* game montagles
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rust", meta = (AllowPrivateAccess = "true"))
		TMap<FName, FMontageConfig> GameMontages;
	/*
	* creature templates
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rust", meta = (AllowPrivateAccess = "true"))
		TMap<FName, TSubclassOf<ARustPawn>> CreatureTemplates;
	/*
	* Object Templates
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rust", meta = (AllowPrivateAccess = "true"))
		TMap<FName, TSubclassOf<ARustActor>> WorldObjectTemplates;
	/*
	* character Templates
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rust", meta = (AllowPrivateAccess = "true"))
		TMap<FName, TSubclassOf<ARustCharacter>> CharacterTemplates;

};
