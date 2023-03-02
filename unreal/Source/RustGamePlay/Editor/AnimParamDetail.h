// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#if WITH_EDITOR || true
#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "UObject/NoExportTypes.h"
#include "../RustAnimInstance.h"
/**
 * https://blog.csdn.net/justtestlike/article/details/84894896
 *
 * struct ”√IPropertyTypeCustomization
 * class ”√ IDetailCustomization
 */
class PROJECTU_API FAnimParamDetail : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
protected:
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
private:
	void OnTypeChanged();
	void OnValueChanged();
	EVisibility IsVisible(EAnimParamType Type);
private:
	TSharedPtr<IPropertyHandle> ValueHandle;
	TSharedPtr<IPropertyHandle> TypeHandle;
	TSharedPtr<IPropertyHandle> MainHandle;

};


#endif
