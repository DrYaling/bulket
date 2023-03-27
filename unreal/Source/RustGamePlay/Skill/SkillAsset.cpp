// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAsset.h"
#include "../FFI/RustPlugin.h"
#include "../RustGameInstance.h"
#include "../GameUnit/RustCharacter.h"
#include "../GameUnit/RustActor.h"
#include "Animation/AnimCompositeBase.h"
#include "../Animation/RustAnimNotifyState.h"
#include "../Animation/RustAnimInstance.h"

class SkillNativeHandlers
{
public:
	bool Valid = false;
	using skill_anim_notify_handler = void(*)(FAnimNotifyValue, void*);
	using set_skill_config_handler = void (*)(FSkillConfig, void*);
	using get_skill_handler = void* (*)(int32, int32);
	using release_skill_handler = void(*)(void*);
	using get_skill_combo_info_handler = FSkillComboState(*)(void*);
	using set_skill_combo_info_handler = void(*)(void*, FSkillComboState);
	using skill_event_delegate = void(*)(ESkillNormalCastState, int32, void*);
	using set_skill_state_delegate_handler = void(*)(void*, skill_event_delegate, void*);
	using get_skill_normal_cast_state_handler = FSkillCastState(*)(void*);
	set_skill_state_delegate_handler set_skill_state_delegate;
	set_skill_config_handler set_skill_config;
	get_skill_handler get_skill;
	release_skill_handler release_skill;
	get_skill_combo_info_handler get_skill_combo_info;
	set_skill_combo_info_handler set_skill_combo_info;
	get_skill_normal_cast_state_handler get_skill_normal_cast_state;
	skill_anim_notify_handler skill_anim_notify;
};
static SkillNativeHandlers Handlers;
extern "C" {
	void SkillEventCallback(ESkillNormalCastState State, int32 Frames, void* Skill);
}
void SkillEventCallback(ESkillNormalCastState State, int32 Frames, void* Skill) {
	if (Skill)
	{
		USkillAsset* SkillAsset = (USkillAsset*)Skill;
		SkillAsset->OnSkillStateChanged(State, Frames);
	}
}


USkillAsset::USkillAsset(): UseCustomBlendSettings(false), SkillHandler(nullptr){
	CurrentCastState.State = ESkillNormalCastState::None;
	BlendSettings.Blend.BlendOption = EAlphaBlendOption::ExpInOut;
	BlendSettings.Blend.BlendTime = 0.25f;
	BlendSettings.BlendMode = EMontageBlendMode::Standard;
}
void USkillAsset::BeginDestroy()
{
	UObject::BeginDestroy();
	if (Handlers.Valid && SkillHandler && OwnerUnit.GetObject() != nullptr) {
		//Handlers.release_skill(SkillHandler); // crash, skill should be destroyed manully
		Handlers.Valid = false;
	}
	//TODO
	SkillHandler = nullptr;
}

void USkillAsset::ProcessTick(float DeltaTime)
{
	//switch (CurrentCastState.State)
	//{
	//	case ESkillNormalCastState::SkillCombo:
	//		{
	//			break;
	//		}
	//	case ESkillNormalCastState::Cooling:
	//		{
	//		break;
	//		}
	//	case ESkillNormalCastState::SkillCasting:
	//		{
	//		break;
	//		}
	//	case ESkillNormalCastState::SkillPreAnim:
	//		{
	//		break;
	//		}
	//	case ESkillNormalCastState::PreTerminate:
	//		{
	//		break;
	//		}
	//	case ESkillNormalCastState::SkillTail:
	//		{
	//		break;
	//		}
	//}
}


void USkillAsset::InitSkillAsset(IRustObjectInterface* Owner, int32 Id)
{
	if (!Owner)
		return;
	SkillId = Id;
	this->OwnerUnit.SetObject(Cast<AActor>(Owner));
	this->OwnerUnit.SetInterface(Owner);
	const auto SGame = sGameInstance;
	if (!SGame)
		return;
	if (SGame->GetPlugin() == nullptr || !SGame->GetPlugin()->IsLoaded()) {
		return;
	}
	if(!Handlers.Valid) {
		const auto RustPlugin = SGame->GetPlugin();
		Handlers.get_skill = (SkillNativeHandlers::get_skill_handler)RustPlugin->GetDllExport(TEXT("get_skill"));
		Handlers.get_skill_combo_info = (SkillNativeHandlers::get_skill_combo_info_handler)RustPlugin->GetDllExport(TEXT("get_skill_combo_info"));
		Handlers.get_skill_normal_cast_state = (SkillNativeHandlers::get_skill_normal_cast_state_handler)RustPlugin->GetDllExport(TEXT("get_skill_normal_cast_state"));
		Handlers.set_skill_combo_info = (SkillNativeHandlers::set_skill_combo_info_handler)RustPlugin->GetDllExport(TEXT("set_skill_combo_info"));
		Handlers.release_skill = (SkillNativeHandlers::release_skill_handler)RustPlugin->GetDllExport(TEXT("release_skill"));
		Handlers.set_skill_state_delegate = (SkillNativeHandlers::set_skill_state_delegate_handler)RustPlugin->GetDllExport(TEXT("set_skill_state_delegate"));
		Handlers.set_skill_config = (SkillNativeHandlers::set_skill_config_handler)RustPlugin->GetDllExport(TEXT("set_skill_config"));
		Handlers.skill_anim_notify = (SkillNativeHandlers::skill_anim_notify_handler)RustPlugin->GetDllExport(TEXT("skill_anim_notify"));
		Handlers.Valid = true;
	}
	if (Handlers.Valid) {
		//this is unsafe unless USkillAsset's lifetime is longer than the owner(Unit) 
		SkillHandler = Handlers.get_skill(Owner->GetUnitState().Uuid, SkillId);
		Handlers.set_skill_state_delegate(SkillHandler, &SkillEventCallback, this);
		UE_LOG(LogNative, Display, TEXT("Skill %d initialize"), SkillId);
		OnSkillInitialize();
	}
}

float USkillAsset::PlayMontage(float InPlayRate, FName StartSectionName)
{
	UAnimMontage* Montage = nullptr;
	if(!SkillMontage.IsNull())
	{
		if(!SkillMontage.IsValid())
			SkillMontage.LoadSynchronous();
		Montage = SkillMontage.Get();
#if WITH_EDITOR
		if (!SkillMontage.IsValid()) {
			UE_LOG(LogNative, Warning, TEXT("Montage %s Load Fail "), *SkillMontage->GetName(), *SkillMontage.ToSoftObjectPath().ToString());
			Montage = Cast<UAnimMontage>(SkillMontage.ToSoftObjectPath().TryLoad());
		}
		/*else {
			UE_LOG(LogNative, Display, TEXT("Montage %s Load Success, value %p, %p"), *SkillMontage->GetName(), SkillMontage.Get(), Montage);
		}*/
#endif
	}
	if (!Montage)
		return 0.0f;
	//if(const auto Character = Cast<ARustCharacter>(OwnerUnit.GetObject()))
	//{
	//	return Character->PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
	//}
	if (const auto AnimInstance = OwnerUnit->GetAnimInstance())
	{
		float Duration;
		//rate override by anim instance
		float AnimRate = AnimInstance->GetAnimPlayRate();
		if (FMath::Abs(AnimRate - 1.0) > 0.001) {
			InPlayRate = AnimRate;
		}
		if(UseCustomBlendSettings)
		{
			Duration = AnimInstance->Montage_PlayWithBlendSettings(Montage, BlendSettings, InPlayRate);
		}
		else
		{
			Duration = AnimInstance->Montage_Play(Montage, InPlayRate);
		}
		

		if (Duration > 0.f)
		{
			// Start at a given Section.
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, Montage);
			}

			return Duration;
		}
	}
	return 0.f;
}

void USkillAsset::SetSkillConfig(FSkillConfig Config, TArray<int32> ComboSkills)
{
	for(size_t index = 0; index < 4 && index < ComboSkills.Num(); ++index)
	{
		Config.ComboSkills[index] = ComboSkills[index];
	}
	Config.ComboCount = FMath::Min(ComboSkills.Num(), 4);
	void* Handler = SkillHandler;
#if WITH_EDITOR
	UE_LOG(LogNative, Display, TEXT("Set Skill %d config,combo %d %d"), SkillId, Config.ComboCount, ComboSkills.Num());
#endif
	if(Handler && Handlers.Valid)
	{
		Handlers.set_skill_config(Config, Handler);
	}
}

void USkillAsset::UnloadSkill()
{
	if (Handlers.Valid && SkillHandler && OwnerUnit.GetObject() != nullptr) {
		Handlers.release_skill(SkillHandler);
	}
	//TODO
	SkillHandler = nullptr;
}

void USkillAsset::OnSkillStateChanged(ESkillNormalCastState State, float Duration)
{
//#if WITH_EDITOR
//	UE_LOG(LogNative, Display, TEXT("OnSkillStateChanged to %d, %f"), State, Duration);
//#endif
	CurrentCastState.StateDuration = Duration;
	CurrentCastState.State = State;
	OnSkillEvent(State, Duration);
}

int32 USkillAsset::GetComboSkill(int32 index)
{
	if (index >= 0 && index < CurrentCombo.ComboCount)
		return CurrentCombo.combo_skills[index];
	return -1;
}

FSkillComboState& USkillAsset::GetComboState()
{
	//owner有效
	if (Handlers.Valid && SkillHandler && OwnerUnit.GetObject() != nullptr)
		CurrentCombo = Handlers.get_skill_combo_info(SkillHandler);
	return CurrentCombo;
}



void USkillAsset::LoadConfigFromMontage(FName StartSectionName)
{
	FSkillConfig Config;
	UAnimMontage* Montage = nullptr;
	if (!SkillMontage.IsNull())
	{
		if(!SkillMontage.IsValid())
		{
			SkillMontage.LoadSynchronous();
		}
		Montage = SkillMontage.Get();
#if WITH_EDITOR
		if (!SkillMontage.IsValid()) {
			UE_LOG(LogNative, Warning, TEXT("Montage %s Load Fail "), *SkillMontage->GetName(), *SkillMontage.ToSoftObjectPath().ToString());
			Montage = Cast<UAnimMontage>(SkillMontage.ToSoftObjectPath().TryLoad());
		}
	/*	else {
			UE_LOG(LogNative, Display, TEXT("Montage %s Load Success, value %p, %p"), *SkillMontage->GetName(), SkillMontage.Get(), Montage);
		}*/
#endif
	}
	if (!Montage)
	{
		UE_LOG(LogNative, Warning, TEXT("Skill %s Montage not found"), *GetName());
		return ;
	}
	struct NotifyState
	{
		URustAnimNotifyState* State;
		float StartTime;
		float Duration;
	};
	TArray<NotifyState> NotifyEvents;
	//TODO
	//AnimMontage.
	float StartTime = .0f;
	float EndTime;
	if(StartSectionName != NAME_None)
	{
		int32 SectionIndex = Montage->GetSectionIndex(StartSectionName);
		if (SectionIndex == -1)
		{
			UE_LOG(LogNative, Warning, TEXT("Section %s not exist"), *StartSectionName.ToString());
			return ;
		}
		Montage->GetSectionStartAndEndTime(SectionIndex, StartTime, EndTime);
		const auto& Notifies = Montage->Notifies;
		for (auto index = 0; index < Notifies.Num(); ++index)
		{
			const auto& Notify = Notifies[index];
			float BeginTime = Notify.GetTriggerTime();
			float Duration = Notify.GetDuration();
			float NotifyEnd = Duration + BeginTime;
			if(BeginTime >= StartTime && NotifyEnd <= EndTime)
			{
				if (const auto State = Cast<URustAnimNotifyState>(Notify.NotifyStateClass))
				{
					//UE_LOG(LogNative, Display, TEXT("Notify  time %f-%f"), BeginTime, NotifyEnd);
					NotifyState Value;
					Value.State = State;
					Value.StartTime = BeginTime;
					Value.Duration = Duration;
					NotifyEvents.Add(Value);
				}
			}
		}
	}
	else
	{
		EndTime = Montage->GetPlayLength();
		const auto& Notifies = Montage->Notifies;
		for (auto index = 0; index < Notifies.Num(); ++index)
		{
			const auto& Notify = Notifies[index];
			if (const auto State = Cast<URustAnimNotifyState>(Notify.NotifyStateClass))
			{
				//UE_LOG(LogNative, Display, TEXT("Notify whole %d time %f"), index, Notify.GetTriggerTime());
				NotifyState Value;
				Value.State = State;
				Value.StartTime = Notify.GetTriggerTime();
				Value.Duration = Notify.GetDuration();
				NotifyEvents.Add(Value);
			}
		}
	}
	//UE_LOG(LogNative, Display, TEXT("Notify StartTime %f, EndTIme %f"), StartTime, EndTime);
	EAnimNotifyType PreviousState = EAnimNotifyType::None;
	int DamageRangeIndex = 0;
	for(const NotifyState Notify : NotifyEvents)
	{
		Notify.State->SkillInstance = this;
		const FAnimNotifyValue& NotifyValue = Notify.State->NotifyValue;
		switch (NotifyValue.NotifyType)
		{
			case EAnimNotifyType::PrevAnimEnd:
			{
				if(PreviousState == EAnimNotifyType::None)
				{
					Config.PreState = Notify.StartTime - StartTime;
					StartTime = Notify.StartTime;
					//UE_LOG(LogNative, Display, TEXT("蒙太奇[%s]动画状态PrevAnimEnd时间%f-%f"), *Montage->GetName(), StartTime, Config.PreState);
					PreviousState = EAnimNotifyType::PrevAnimEnd;
				}
				else
				{
					UE_LOG(LogNative, Error, TEXT("蒙太奇动画状态错误,PrevAnim状态在其它状态%d之后"), PreviousState);
					return ;
				}
				break;
			}
			case EAnimNotifyType::PreTerminateEnd:
			{
				if (PreviousState == EAnimNotifyType::None || PreviousState == EAnimNotifyType::PrevAnimEnd)
				{
					Config.PreTerminate = Notify.StartTime - StartTime;
					StartTime = Notify.StartTime;
					PreviousState = EAnimNotifyType::PreTerminateEnd;
					//UE_LOG(LogNative, Display, TEXT("蒙太奇[%s]动画状态PreTerminateEnd时间%f-%f"), *Montage->GetName(), StartTime, Config.PreTerminate);
				}
				else
				{
					UE_LOG(LogNative, Error, TEXT("蒙太奇动画状态错误,Terminate状态在其它状态%d之后"), PreviousState);
					return ;
				}
				break;
			}
			case EAnimNotifyType::SkillCastingEnd:
			{
				if (PreviousState == EAnimNotifyType::None || PreviousState == EAnimNotifyType::PrevAnimEnd || PreviousState == EAnimNotifyType::PreTerminateEnd)
				{
					Config.Casting = Notify.StartTime - StartTime;
					StartTime = Notify.StartTime;
					//UE_LOG(LogNative, Display, TEXT("蒙太奇[%s]动画状态SkillCastingEnd时间%f-%f"), *Montage->GetName(), StartTime, Config.Casting);
					PreviousState = EAnimNotifyType::SkillCastingEnd;
				}
				else
				{
					UE_LOG(LogNative, Error, TEXT("蒙太奇动画状态错误,Cast状态在其它状态%d之后"), PreviousState);
					return ;
				}
				break;
			}
			case EAnimNotifyType::SkillComboEnd:
			{
				if (PreviousState == EAnimNotifyType::SkillCastingEnd)
				{
					Config.ComboDura = Notify.StartTime - StartTime;
					StartTime = Notify.StartTime;
					//UE_LOG(LogNative, Display, TEXT("蒙太奇[%s]动画状态SkillComboEnd时间%f-%f"), *Montage->GetName(), StartTime, Config.ComboDura);
					PreviousState = EAnimNotifyType::SkillCastingEnd;
				}
				else
				{
					UE_LOG(LogNative, Error, TEXT("蒙太奇动画状态错误, 没有Cast状态"));
					return ;
				}
				break;
			}
			case EAnimNotifyType::SkillFinish:
			{
				if (PreviousState == EAnimNotifyType::SkillCastingEnd || PreviousState == EAnimNotifyType::SkillComboEnd)
				{
					Config.Tail = Notify.StartTime - StartTime;
					StartTime = Notify.StartTime;
					//UE_LOG(LogNative, Display, TEXT("蒙太奇[%s]动画状态SkillFinish时间%f-%f"), *Montage->GetName(), StartTime, Config.Tail);
					PreviousState = EAnimNotifyType::SkillFinish;
				}
				else
				{
					UE_LOG(LogNative, Error, TEXT("蒙太奇动画状态错误, 没有Cast状态"));
					return ;
				}
				break;
			}
			case EAnimNotifyType::SkillDamageCheck:
			{
				if (DamageRangeIndex >= 2) {
					UE_LOG(LogNative, Warning, TEXT("Skill %s damge check range got %d"), *GetName(), DamageRangeIndex);
				}
				else {
					Config.DamageRanges[DamageRangeIndex * 2] = Notify.StartTime;
					Config.DamageRanges[DamageRangeIndex * 2 + 1] = Notify.StartTime + Notify.Duration;
				}
				DamageRangeIndex++;			
				break;
			}
			default: 
				break;
		}
	}
	if(PreviousState != EAnimNotifyType::SkillFinish)
	{
		UE_LOG(LogNative, Error, TEXT("蒙太奇[%s]动画状态错误,没有结束通知"), *Montage->GetName());
		return;
	}
	//else {
	//	UE_LOG(LogNative, Display, TEXT("蒙太奇[%s]动画状态初始化成功"), *Montage->GetName());
	//}
	SetSkillConfig(Config, DefaultComboSkills);
}

void USkillAsset::OnAnimNotify(FAnimNotifyValue NotifyValue)
{
#if WITH_EDITOR
	UE_LOG(LogNative, Display, TEXT("OnAnimNotify type %d, value %d, begin %d"), NotifyValue.NotifyType, NotifyValue.Value, NotifyValue.IsBegin);
#endif
	if (SkillHandler && OwnerUnit.GetObject() != nullptr && Handlers.Valid) { 
		Handlers.skill_anim_notify(NotifyValue, SkillHandler);
	}
}

void USkillAsset::UnloadHandlers()
{
	Handlers.Valid = false;
}
