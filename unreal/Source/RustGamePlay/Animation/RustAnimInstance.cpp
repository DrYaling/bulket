#include "RustAnimInstance.h"
#include "../TransformLayer.h"
#include "../FFI/RustApi.h"
#include "../GameUnit/RustCharacter.h"
#include "../GameUnit/RustPawn.h"
FAnimParams::FAnimParams(): ParamType(EAnimParamType::Bool)
{
	SetBool(false);
}

FAnimParams::FAnimParams(EAnimParamType Pt): ParamType(Pt)
{
	switch (Pt)
	{
	case EAnimParamType::Bool:
		SetBool(false);
		break;
	case EAnimParamType::Float:
		SetFloat(0.0);
		break;
	case EAnimParamType::Integer:
		SetInt(0);
		break;
	case EAnimParamType::Vector:
		SetVector(FVector::Zero());
		break;
	default:
		break;
	}
}


void URustAnimInstance::NativeInitializeAnimation()
{
	InCombat = false;
	HitState.IsHit = false;
	IsDead = false;
	BaseMovement = nullptr;
	CharacterMovement = nullptr;
	AnimPlayRate = 1.0f;
	const auto TargetActor = GetOwningActor();
	if (const auto Character = Cast<ARustCharacter>(TargetActor)) {
		CharacterMovement = Character->GetCharacterMovement();
		BaseMovement = CharacterMovement;
	}
	else if (const auto Pawn = Cast<ARustPawn>(TargetActor)) {
		BaseMovement = Pawn->GetMovementComponent();
	}
}

void URustAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	//freeze state
	if(bFreezing)
	{
		FreezingDuration -= DeltaSeconds;
		if (FreezingDuration <= 0.0)
			bFreezing = false;
		return;
	}
	//move dir and speed
	if (BaseMovement) {
		Velocity = BaseMovement->Velocity;
		MoveSpeed = Velocity.Length();
		bool Falling = IsFalling;
		IsFalling = BaseMovement->IsFalling();
		bool Moving = ShouldMove;
		ShouldMove = MoveSpeed >= MoveSpeedThreshold;
		if (Falling != IsFalling)
			OnAnimStateChanged(EGameAnimStateType::IsFalling, IsFalling);
		if (Moving != ShouldMove)
			OnAnimStateChanged(EGameAnimStateType::ShouldMove, ShouldMove);
	}
	//acces
	if (CharacterMovement) {
		const auto Accel = CharacterMovement->GetCurrentAcceleration();
		MoveDir = FVector2f(Accel.X, Accel.Y);
		Acceleration = MoveDir.Length();
		if (Acceleration > 0.1 && MoveSpeed > MoveSpeedThreshold) {
			MoveDir.Normalize();
		}
	}
	if (AnimPlayRateDuration > 0) {
		AnimPlayRateTick += DeltaSeconds;
		if (AnimPlayRateTick >= AnimPlayRateDuration) {
			AnimPlayRateTick = 0.0;
			AnimPlayRateDuration = -1.0;
			AnimPlayRate = 1.0;
			UE_LOG(LogNative, Display, TEXT("Tick Play Rate passed %f, %f, %f"), AnimPlayRateDuration, AnimPlayRateTick, AnimPlayRate);
			UpdateMontageRate(1.0);
			OnAnimStateChanged(EGameAnimStateType::AnimRate, 1000);
		}
	}
}

void URustAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
}

void URustAnimInstance::SetFixedAnimState(EFixeAnimState State, FAnimParams Param)
{
	switch (State)
	{
	case EFixeAnimState::InCombat:
	{
		bool Combat = InCombat;
		InCombat = Param.GetBool();
		if (InCombat != Combat)
			OnAnimStateChanged(EGameAnimStateType::InCombat, InCombat);
		UE_LOG(LogNative, Display, TEXT("SetState Combat to %d, %f"), InCombat, Param.GetFloat());
		break;
	}
	case EFixeAnimState::IsDead:
	{
		bool Dead = IsDead;
		IsDead = Param.GetBool();
		if (Dead != IsDead)
			OnAnimStateChanged(EGameAnimStateType::IsDead, IsDead);
		UE_LOG(LogNative, Display, TEXT("SetState Dead to %d, %f"), IsDead, Param.GetFloat());
		break;
	}
	case EFixeAnimState::StandUp:
	{
		OnAnimStateChanged(EGameAnimStateType::StandUp, 0);
		break;
	}
	case EFixeAnimState::KnockDown:
	{
		OnAnimStateChanged(EGameAnimStateType::KnockDown, 0);
		break;
	}
	default:
		break;
	}
}

void URustAnimInstance::SetDynAnimState(FName StateName, FAnimParams Param)
{
	UE_LOG(LogNative, Display, TEXT("Set Property %s: value: %f,%d"), *StateName.ToString(), Param.Value.X, Param.GetBool());
	if (const auto Property = Properties.Find(StateName)) {
		*Property = Param;
	}
}

void URustAnimInstance::AddBool(FName Name, bool Value)
{
	if (Properties.Contains(Name))
		return;
	FAnimParams Param;
	Param.ParamType = EAnimParamType::Bool;
	Param.SetBool(Value);
	AddValue(Name, Param);
}

void URustAnimInstance::AddFloat(FName Name, float Value)
{
	if (Properties.Contains(Name))
		return;
	FAnimParams Param;
	Param.ParamType = EAnimParamType::Float;
	Param.SetFloat(Value);
	AddValue(Name, Param);
}

void URustAnimInstance::AddInteger(FName Name, int32 Value)
{
	if (Properties.Contains(Name))
		return;
	FAnimParams Param;
	Param.ParamType = EAnimParamType::Integer;
	Param.SetInt(Value);
	AddValue(Name, Param);
}

void URustAnimInstance::AddVector(FName Name, FVector Value)
{
	if (Properties.Contains(Name))
		return;
	FAnimParams Param;
	Param.ParamType = EAnimParamType::Vector;
	Param.SetVector(Value);
	AddValue(Name, Param);
}

void URustAnimInstance::AddValue(FName Name, FAnimParams Value)
{
	if(Properties.Contains(Name))
	{
		Properties[Name] = Value;
	}
	else
	{
		Properties.Add(Name, Value);
	}
}

void URustAnimInstance::SetBool(FName Name, bool In)
{
	if (const auto Value = Properties.Find(Name))
	{
		if (Value->ParamType == EAnimParamType::Bool)
			Value->SetBool(In);
	}
}

void URustAnimInstance::SetFloat(FName Name, float In)
{
	if (const auto Value = Properties.Find(Name))
	{
		if (Value->ParamType == EAnimParamType::Float)
			Value->SetFloat(In);
	}
}

void URustAnimInstance::SetInteger(FName Name, int32 In)
{
	if (const auto Value = Properties.Find(Name))
	{
		if (Value->ParamType == EAnimParamType::Integer)
			Value->SetInt(In);
	}
}

void URustAnimInstance::SetVector(FName Name, FVector Fv)
{
	if (const auto Value = Properties.Find(Name))
	{
		if (Value->ParamType == EAnimParamType::Vector)
			Value->SetVector(Fv);
	}
}

void URustAnimInstance::SetValue(FName Name, FAnimParams Value)
{
	if (Properties.Contains(Name))
	{
		Properties[Name] = Value;
	}
}
static FAnimParams Default = FAnimParams(EAnimParamType::Bool);
const FAnimParams& URustAnimInstance::GetValue(FName Name)
{

	if (Properties.Contains(Name))
	{
		return Properties[Name];
	}
	return Default;
}

bool URustAnimInstance::GetBoolValue(FName Name) const
{
	if(const auto Value = Properties.Find(Name))
	{
		if (Value->ParamType == EAnimParamType::Bool)
			return Value->GetBool();
	}
	return false;
}

float URustAnimInstance::GetFloatValue(FName Name) const
{
	if (const auto Value = Properties.Find(Name))
	{
		if (Value->ParamType == EAnimParamType::Float)
			return Value->GetFloat();
	}
	return 0.0f;
}

int32 URustAnimInstance::GetIntValue(FName Name) const
{
	if (const auto Value = Properties.Find(Name))
	{
		if (Value->ParamType == EAnimParamType::Integer)
			return Value->GetInt();
	}
	return -1;
}

bool URustAnimInstance::ConsumeBoolValue(FName Name)
{
	if (const auto Value = Properties.Find(Name))
	{
		if (Value->ParamType == EAnimParamType::Bool && Value->GetBool())
		{
			Value->SetBool(false);
			return true;
		}
	}
	return false;
}

void URustAnimInstance::SetAnimPlayRate(float Rate, float Duration)
{
	AnimPlayRate = Rate;
	AnimPlayRateDuration = Duration;
	AnimPlayRateTick = 0.0;
	UpdateMontageRate(Rate);
	OnAnimStateChanged(EGameAnimStateType::AnimRate, Rate * 1000000);
	UE_LOG(LogNative, Display, TEXT("Set Play Rate %f, %f, %f"), AnimPlayRateDuration, AnimPlayRateTick, AnimPlayRate);
}


void URustAnimInstance::OnAnimStateChanged_Implementation(EGameAnimStateType State, int32 Value)
{
	if(State != EGameAnimStateType::ShouldMove)
		UE_LOG(LogNative, Display, TEXT("OnAnimStateChanged %d"), State);
	const auto TargetActor = GetOwningActor();
	if (const auto Character = Cast<ARustCharacter>(TargetActor)) {
		Character->OnAnimStateChanged(State, Value);
	}
	else if (const auto Pawn = Cast<ARustPawn>(TargetActor)) {
		Pawn->OnAnimStateChanged(State, Value);
	}
}

void URustAnimInstance::UpdateMontageRate(float Rate)
{
	if (IsAnyMontagePlaying()) {
		for (const auto Montage : MontageInstances) {
			Montage->SetPlayRate(Rate);
		}
	}
}


void URustAnimInstance::SetFreeze(float Duration, bool OverrideDuration)
{
	if (Duration <= 0.0)
		return;
	bFreezing = true;
	if (OverrideDuration)
		FreezingDuration = Duration;
	else
		FreezingDuration += Duration;
	UpdateMontageRate(0.0);
}

void URustAnimInstance::SetNotFreeze()
{
	bFreezing = false;
	FreezingDuration = 0.0;
	UpdateMontageRate(AnimPlayRate);
}