// Fill out your copyright notice in the Description page of Project Settings.


#include "GameConfig.h"
#include "Animation/AnimMontage.h"
#include "GameUnit/RustCharacter.h"
#include "GameUnit/RustActor.h"
#include "GameUnit/RustPawn.h"
#include "UI/GameWidget.h"
UAnimMontage* UGameConfig::GetMontage(FName Name)
{
	if (auto Class = GameMontages.Find(Name)) {
		if (Class->Montage.IsNull())
			return nullptr;
		if(!Class->Montage.IsValid())
			Class->Montage.LoadSynchronous();
		return Class->Montage.Get();
	}
	return nullptr;
}

TSoftObjectPtr<UAnimMontage> UGameConfig::GetMontageAsset(FName Name) const
{
	auto Kv = GameMontages.Find(Name);
	return Kv != nullptr ? Kv->Montage : nullptr;
}

TSubclassOf<ARustActor> UGameConfig::GetWorldObjectTemplate(FName Name) const
{
	auto Template = WorldObjectTemplates.Find(Name);
	return Template != nullptr ? *Template : TSubclassOf<ARustActor>();
}

TSubclassOf<ARustPawn> UGameConfig::GetCreatureTemplate(FName Name) const
{
	auto Template = CreatureTemplates.Find(Name);
	return Template != nullptr ? *Template : TSubclassOf<ARustPawn>();
}

TSubclassOf<ARustCharacter> UGameConfig::GetCharacterTemplate(FName Name) const
{
	auto Template = CharacterTemplates.Find(Name);
	return Template != nullptr ? *Template : TSubclassOf<ARustCharacter>();
}
