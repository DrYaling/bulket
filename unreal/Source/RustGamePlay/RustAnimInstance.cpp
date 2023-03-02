#include "RustAnimInstance.h"
#include "TransformLayer.h"
#include "RustApi.h"

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
}

void URustAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	//
	
}

void URustAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
}

void URustAnimInstance::SetFixedAnimState(EFixeAnimState State, FAnimParams Param)
{
	switch (State)
	{
	case EFixeAnimState::InCombat:
		InCombat = Param.GetBool();
		UE_LOG(LogNative, Display, TEXT("SetState Combat to %b"), InCombat);
		break;
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
