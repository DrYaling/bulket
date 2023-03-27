#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DetailCategoryBuilder.h"
#include "Templates/SubclassOf.h"
#include "UObject/Object.h"
#include "../FFI/FFITypes.h"
#include "RustProperty.generated.h"
DECLARE_DELEGATE_RetVal(FReply, FOnComponentRemoved);
class URustAnimInstance;
UENUM()
enum ERustPropertyTag
{
	Bool,
	Float,
	Vector,
	Quat,
	Class,
	Sound,
	SkeletonMesh,
	StaticMesh,
};

enum class ReflectionType : uint32_t {
	Float,
	Vector3,
	Bool,
	Quaternion,
	UClass,
	USound,
	USkeleton,
	UStaticMesh,
	Composite,
};
// TODO: This is a disgusting hack. We store all the possible variants in this struct so that we can access them
// via the property system. The reason for that is `IPropertyHandle::SetValue` only implements a few overrides
// like for FVector, FRotator etc. It would have been nice if we could set values for non default types.
// There is `IPropertyHandle::AccessRawData` which we can use to write any data we want. But using this doesn't
// update all instances of this property. Eg if we edit the blueprint base class, but we already placed this class in
// a level, none of the properties would update for the blueprint classes that were already placed in the level.
// But `IPropertyHandle::SetValue` seems to support that.
USTRUCT()
struct FRustProperty
{
	GENERATED_BODY()

		//UPROPERTY(EditAnywhere)
		//TEnumAsByte<ERustPropertyTag> Tag;
	union
	{
	UPROPERTY(EditAnywhere, Category = Rust)
		float Float;

	UPROPERTY(EditAnywhere, Category = Rust)
		bool Bool;

	UPROPERTY(EditAnywhere, Category = Rust)
		FVector Vector;

	UPROPERTY(EditAnywhere, Category = Rust)
		FRotator Rotation;

	UPROPERTY(EditAnywhere, Category = Rust)
		TSubclassOf<AActor> Class;

	UPROPERTY(EditAnywhere, Category = Rust)
		TObjectPtr<USoundBase> Sound;
	UPROPERTY(EditAnywhere, Category = Rust)
		TObjectPtr<USkeletalMesh> Skeletal;
	UPROPERTY(EditAnywhere, Category = Rust)
		TObjectPtr<UStaticMesh> StaticMesh;
	};
	UPROPERTY(EditAnywhere, Category = Rust)
		int32 Tag;
	FRustProperty(): Float(0), Bool(false), Tag(ERustPropertyTag::Bool)
	{
	}

	static void Initialize(TSharedPtr<IPropertyHandle> Handle, ReflectionType Type);
};

USTRUCT()
struct FDynamicRustComponent
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, Category = Rust)
		TMap<FString, FRustProperty> Fields;

	UPROPERTY(EditAnywhere, Category = Rust)
		FString Name;

	void Reload(TSharedPtr<IPropertyHandle> Handle, FGuid Guid);
	// Initializes the property handle. It sets the same, and adds all the fields to the hashmap.
	static void Initialize(TSharedPtr<IPropertyHandle> Handle, FGuid InitGuid);
	static void Render(TSharedRef<IPropertyHandle> MapHandle, IDetailCategoryBuilder& DetailBuilder,
		IDetailLayoutBuilder& LayoutBuilder);
};
/**
 * rust game play unit state
 */
USTRUCT(BlueprintType)
struct FUnitState
{
	GENERATED_BODY()
	/**
	 * @brief spawn location of X
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SpawnLocationX;
	/**
	 * @brief spawn location of Y
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SpawnLocationY;
	/**
	 * @brief spawn location of Z
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SpawnLocationZ;
	/**
	 * @brief owner of this state
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* Owner;
	/**
	 * @brief spawn scale
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SpawnScale;

	/**
	 * @brief Unit template Id
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 TemplateID;

	/**
	 * @brief Unit Id
	 * based on UnitType
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 UnitId;
	/**
	 * @brief level Id if it's creature or worldObject
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 LevelId;
	/**
	 * @brief unit type of this unit
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		ERustUnitType UnitType;
	/**
	 * object uuid of game
	 */
	int32 Uuid;
	/*
	* animation of this object
	*/
	UPROPERTY(BlueprintReadOnly)
	URustAnimInstance* Animation;
	FUnitState();
	FUnitState(ERustUnitType Et);
	FUnitState& FUnitState::operator =(const FUnitState& Other) {
		Uuid = Other.Uuid;
		Animation = Other.Animation;
		LevelId = Other.LevelId;
		Owner = Other.Owner;
		SpawnLocationX = Other.SpawnLocationX;
		SpawnLocationY = Other.SpawnLocationY;
		SpawnLocationZ = Other.SpawnLocationZ;
		SpawnScale = Other.SpawnScale;
		UnitType = Other.UnitType;
		UnitId = Other.UnitId;
		TemplateID = Other.TemplateID;
		return *this;
	}
	static const FUnitState& Default();
};