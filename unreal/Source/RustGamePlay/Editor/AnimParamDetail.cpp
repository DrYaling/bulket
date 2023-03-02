// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimParamDetail.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "../RustAnimInstance.h"
// engine header
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "EditorStyleSet.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SNumericDropDown.h"
#include "Widgets/Input/SVectorInputBox.h"
#include "../RustApi.h"
#define LOCTEXT_NAMESPACE "FAnimParamDetail"


TSharedRef<IPropertyTypeCustomization> FAnimParamDetail::MakeInstance()
{
	return MakeShareable(new FAnimParamDetail());
}

FVector GetFloat(float Float)
{
    FVector Value;
    Value.X = Float;
    return Value;
}
FVector GetInt(int32 Int)
{
    FVector Value;
    Value.X = (double)(*&Int);
    return Value;
}
float CheckFloat(const FVector& V) {
    return V.X;
}
int32 CheckInt(const FVector& V) {
    return (int32)(*&V.X);
}
bool CheckBool(const FVector& V) {
    return CheckInt(V) != 0;
}
void FAnimParamDetail::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    if (!PropertyHandle->IsValidHandle())
        return;
    HeaderRow.NameContent()[PropertyHandle->CreatePropertyNameWidget()]
        .ValueContent()[
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .MaxWidth(300)
                [
                    SNew(STextBlock)
                    .Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
                .Text(LOCTEXT("ParamType", "AnimarionState"))
                ]
        ];
}

void FAnimParamDetail::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    if (!PropertyHandle->IsValidHandle())
    {
        UE_LOG(LogNative, Warning, TEXT("PropertyHandle Invalid"));
        return;
    }
    TSharedPtr<IPropertyHandle> TypePropertyHandle = PropertyHandle->GetChildHandle(
        GET_MEMBER_NAME_CHECKED(FAnimParams, ParamType));

    TypePropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FAnimParamDetail::OnTypeChanged));
    if (!TypePropertyHandle.IsValid())
    {
        UE_LOG(LogNative, Error, TEXT("Fail to read ParamType"));
        return;
    }
    TypeHandle = TypePropertyHandle;
    FString Name;
    TypePropertyHandle->GetValue(Name);
    uint8 data;
    auto Result = TypePropertyHandle->GetValue(data);
    if (Result != FPropertyAccess::Result::Success)
    {
        UE_LOG(LogNative, Error, TEXT("Fail to read ParamType %s"), *Name);
        return;
    }
    EAnimParamType EType = (EAnimParamType)data;
    ChildBuilder.AddCustomRow(LOCTEXT("ParamType", "ParamType"))[
        SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .MaxWidth(300)
            [
                SNew(STextBlock)
                .Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
            .Text(LOCTEXT("ParamType", "ParamType"))
            ]
            .Padding(5)
                +SHorizontalBox::Slot()
                .MaxWidth(300)
                [
                    TypePropertyHandle->CreatePropertyValueWidget()
                ]
    ];
    TSharedPtr<IPropertyHandle> Property = PropertyHandle->GetChildHandle(
        GET_MEMBER_NAME_CHECKED(FAnimParams, Value));
    if (!Property.IsValid())
    {
        UE_LOG(LogNative, Warning, TEXT("ValueHandle Invalid"));
        return;
    }
    ValueHandle = Property;
    ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FAnimParamDetail::OnValueChanged));
    ChildBuilder.AddCustomRow(LOCTEXT("Value", "Value"))[
        SNew(SVerticalBox)
        +SVerticalBox::Slot()
         [
                SNew(SHorizontalBox)
                .Visibility_Lambda([this] {
            return IsVisible(EAnimParamType::Bool);
                    })
            + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("Triggered", "Triggered"))
                        ]
                    .Padding(10)
                        + SHorizontalBox::Slot()
                        .MaxWidth(400)
                        [
                            SNew(SCheckBox)
                            .IsChecked_Lambda([this]() {
                        FVector State;
                    if (ValueHandle)
                        ValueHandle->GetValue(State);
                    return CheckBool(State) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                                })
                        .OnCheckStateChanged_Lambda([this](ECheckBoxState State)
                            {
                                if (ValueHandle) {
                                    ValueHandle->SetValue(GetInt(State == ECheckBoxState::Checked));
                                }
                                else
                                    UE_LOG(LogNative, Error, TEXT("Property Invalid"));
                            })
            ]]
        +SVerticalBox::Slot()
            [
                SNew(SHorizontalBox)
                .Visibility_Lambda([this] {
            return IsVisible(EAnimParamType::Float);
                    })
            + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("Value", "Value"))
                        ]
                    .Padding(10)
                        + SHorizontalBox::Slot()
                        .MaxWidth(400)
                        [
                            SNew(SNumericEntryBox<float>)
                            .Value_Lambda([this]() {
                        FVector State;
                    if (ValueHandle)
                        ValueHandle->GetValue(State);
                    return CheckFloat(State);
                                })
                        .OnValueChanged_Lambda([this](float Value)
                            {
                                if (ValueHandle)
                                ValueHandle->SetValue(GetFloat(Value));
                                else
                                    UE_LOG(LogNative, Error, TEXT("Property Invalid"));
                            })
                        ]
        ]
        +SVerticalBox::Slot()
            [
                SNew(SHorizontalBox)
                .Visibility_Lambda([this] {
            return IsVisible(EAnimParamType::Integer);
                    })
            + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("Value", "Value"))
                        ]
                    .Padding(10)
                        + SHorizontalBox::Slot()
                        .MaxWidth(400)
                        [
                            SNew(SNumericEntryBox<int32>)
                            .Value_Lambda([this]() {
                        FVector State;
                    if (ValueHandle)
                        ValueHandle->GetValue(State);
                    return CheckInt(State);
                                })
                        .OnValueChanged_Lambda([this](int32 Value)
                            {
                                if (ValueHandle)
                                ValueHandle->SetValue(GetInt(Value));
                                else
                                    UE_LOG(LogNative, Error, TEXT("Property Invalid"));
                            })
                        ]
            ]
        +SVerticalBox::Slot()
            [
                SNew(SHorizontalBox)
                .Visibility_Lambda([this] {
            return IsVisible(EAnimParamType::Vector);
                    })
            + SHorizontalBox::Slot()
                .AutoWidth()
            [
                        SNew(STextBlock)
                        .Text(LOCTEXT("Value", "Value"))
                    ]
                .Padding(10)
                    + SHorizontalBox::Slot()
                    .MaxWidth(300)
                    [
                        SNew(SNumericVectorInputBox<double>)
                        .Vector_Lambda([this]() {
                    FVector State;
                if (ValueHandle)
                    ValueHandle->GetValue(State);
                return State;
                            })
                    .OnXChanged_Lambda([this](double Value)
                    {
                        UE_LOG(LogNative, Display, TEXT("Vector Changed %f"), Value);
                        FVector State;
                        if (ValueHandle)
                            ValueHandle->GetValue(State);
                        State.X = Value;
                        if (ValueHandle)
                            ValueHandle->SetValue(State);
                        else
                            UE_LOG(LogNative, Error, TEXT("Property Invalid"));
                    })
                            .OnYChanged_Lambda([this](double Value)
                                {
                                    UE_LOG(LogNative, Display, TEXT("Vector Changed %f"), Value);
                    FVector State;
                    if (ValueHandle)
                        ValueHandle->GetValue(State);
                    State.Y = Value;
                    if (ValueHandle)
                        ValueHandle->SetValue(State);
                    else
                        UE_LOG(LogNative, Error, TEXT("Property Invalid"));
                                })
                        .OnZChanged_Lambda([this](double Value)
                            {
                                UE_LOG(LogNative, Display, TEXT("Vector Changed %f"), Value);
                                FVector State;
                                if (ValueHandle)
                                    ValueHandle->GetValue(State);
                                State.Z = Value;
                                if (ValueHandle)
                                    ValueHandle->SetValue(State);
                                else
                                    UE_LOG(LogNative, Error, TEXT("Property Invalid"));
                            })
                ]
            ]
    ];
}
void FAnimParamDetail::OnTypeChanged()
{
    if (ValueHandle) {
        uint8 Type;
        TypeHandle->GetValue(Type);
        auto EType = (EAnimParamType)Type;
        UE_LOG(LogNative, Display, TEXT("Type Changed to %d"), Type);
        ValueHandle->GetParentHandle()->NotifyPreChange();
        ValueHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
        switch (EType) {
        case EAnimParamType::Bool:
            ValueHandle->SetValue(GetInt(0));
            break;
        case EAnimParamType::Float:
            ValueHandle->SetValue(GetFloat(0.0));
            break;
        case EAnimParamType::Integer:
            ValueHandle->SetValue(GetInt(0));
            break;
        case EAnimParamType::Vector:
            ValueHandle->SetValue(FVector::Zero());
            break;
        }
        ValueHandle->ClearResetToDefaultCustomized();
        TypeHandle->NotifyFinishedChangingProperties();
        ValueHandle->GetParentHandle()->NotifyFinishedChangingProperties();
    }
}
void FAnimParamDetail::OnValueChanged()
{
    UE_LOG(LogNative, Display, TEXT("OnValueChanged"));
}
EVisibility FAnimParamDetail::IsVisible(EAnimParamType Type)
{
    if (!TypeHandle)
        return EVisibility::Hidden;
    uint8 EData;
    TypeHandle->GetValue(EData);
    EAnimParamType EType = (EAnimParamType)EData;
    if (EType == Type)
        return EVisibility::All;
    return EVisibility::Hidden;
}
/*
void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    TArray< TSharedPtr<FStructOnScope> > StructBeingCustomized;
    DetailBuilder.GetStructsBeingCustomized(StructBeingCustomized);
    check(StructBeingCustomized.Num() == 1);
    FAnimParams* Params = (FAnimParams*)StructBeingCustomized[0].Get()->GetStructMemory();
    UE_LOG(LogNative, Warning, TEXT("CustomizeDetails %d"), Params->ParamType);
    IDetailCategoryBuilder& VersionCategory = DetailBuilder.EditCategory("Version", FText::GetEmpty(), ECategoryPriority::Default);
    VersionCategory.SetShowAdvanced(true);

    VersionCategory.AddCustomRow(LOCTEXT("ImportPakLists", "Import Pak Lists"), true)
        .ValueContent()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
        .Padding(0)
        .AutoWidth()
        [
            SNew(STextBlock)
        .ToolTipText(LOCTEXT("EAnimParamType", "ParamType"))
        .Text_Lambda([this, Params]()
        {
	        switch (Params->ParamType)
	        {
	        case EAnimParamType::Bool:  FText::FromString(FString("Bool"));
	        case EAnimParamType::Float: FText::FromString(FString("Float"));
	        case EAnimParamType::Integer: FText::FromString(FString("Integer"));
	        case EAnimParamType::Vector: FText::FromString(FString("Vector"));
	        case EAnimParamType::Long: FText::FromString(FString("Long"));
	        default: ;
	        }
			return FText::FromString(FString("Error"));
        })
        ]
    + SHorizontalBox::Slot()
        .Padding(5, 0, 0, 0)
        .AutoWidth()
        [
            SNew(SCheckBox)
        .ToolTipText(LOCTEXT("BoolValue", "BoolValue"))
        .Visibility_Lambda([this, Params]()
            {
                if (Params->ParamType == EAnimParamType::Bool)
					return EVisibility::Visible;
                else
                    return EVisibility::Hidden;
            })
        .OnCheckStateChanged_Lambda([this, Params](ECheckBoxState State)
            {
                if (Params)
					Params->BoolValue = State == ECheckBoxState::Checked;
        })
        ]
    + SHorizontalBox::Slot()
        .Padding(5, 0, 0, 0)
        .AutoWidth()
        [
            SNew(SVectorInputBox)
            .ToolTipText(LOCTEXT("VectorValue", "VectorValue"))
        .Visibility_Lambda([this, Params]()
            {
                if (Params->ParamType == EAnimParamType::Vector)
					return EVisibility::Visible;
                else
                    return EVisibility::Hidden;
            })
        .OnVectorChanged_Lambda([this, Params](UE::Math::TVector<float> Value)
            {
                if (Params)
                {
	                Params->VectorValue = FVector(Value.X, Value.Y, Value.Z);
                }
        })
        ]
    + SHorizontalBox::Slot()
        .Padding(5, 0, 0, 0)
        .AutoWidth()
        [
            SNew(SNumericEntryBox<float>)
            .ToolTipText(LOCTEXT("FloatValue", "FloatValue"))
        .Visibility_Lambda([this, Params]()
            {
                if (Params->ParamType == EAnimParamType::Float)
					return EVisibility::Visible;
                else
                    return EVisibility::Hidden;
            })
        .OnValueChanged_Lambda([this, Params](float Value)
            {
                if (Params)
					Params->FloatValue = Value;
            })
        ]
    + SHorizontalBox::Slot()
        .Padding(5, 0, 0, 0)
        .AutoWidth()
        [
            SNew(SNumericEntryBox<int32>)
            .ToolTipText(LOCTEXT("IntValue", "IntValue"))
        .Visibility_Lambda([this, Params]()
            {
                if (Params->ParamType == EAnimParamType::Integer)
					return EVisibility::Visible;
                else
                    return EVisibility::Hidden;
            })
        .OnValueChanged_Lambda([this, Params](int32 Value)
            {
                if (Params)
					Params->IntValue = Value;
            })
        ]
    + SHorizontalBox::Slot()
        .Padding(5, 0, 0, 0)
        .AutoWidth()
        [
            SNew(SNumericEntryBox<int64>)
            .ToolTipText(LOCTEXT("LongValue", "LongValue"))
        .Visibility_Lambda([this, Params]()
            {
                if (Params->ParamType == EAnimParamType::Long)
					return EVisibility::Visible;
                else
                    return EVisibility::Hidden;
            })
        .OnValueChanged_Lambda([this, Params](int64 Value)
            {
                if (Params)
					Params->LongValue = Value;
            })
        ]
        ];

}
*/
#undef LOCTEXT_NAMESPACE
