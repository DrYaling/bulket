#include "TransformLayer.h"

#include "RustActor.h"
#include "RustGameInstance.h"
AActor* UTransformLayer::SpawnObjectByName(const FString& BPName, const FTransform& Transform, FUnitState UnitState,
	AActor* Owner)
{
	return nullptr;
}

AActor* UTransformLayer::SpawnObjectByBlueprint(TSubclassOf<UClass> character, const FTransform& Transform,
                                                FUnitState UnitState, AActor* Owner)
{
	return nullptr;
}

int32 UTransformLayer::GetNetworkDelay()
{
	return -1;
}

bool UTransformLayer::IsRunAsServer()
{
	return false;
}

URustGameInstance* UTransformLayer::GetGameInstance()
{
	return sGameInstance;
}

const FUnitState& UTransformLayer::GetRustObjectState(const AActor* Actor)
{
	if (const auto RustObject = Cast<IRustObjectInterface>(Actor))
	{
		return RustObject->GetUnitState();
	}
	return FUnitState::Default();
}

ERustUnitType UTransformLayer::GetRustObjectType(const AActor* Actor)
{
	if (const auto RustObject = Cast<IRustObjectInterface>(Actor))
	{
		return RustObject->GetUnitType();
	}
	return ERustUnitType::Undefined;
}

int32 UTransformLayer::GetNameHash(const FString& Name)
{
	const auto Char = Name.GetCharArray().GetData();
	char* DST = (char*)TCHAR_TO_UTF8(Char);
	int32 Hash = 0;
	for (;*DST; ++DST)
	{
		const int64 K = 5 * (int64)Hash + (int64)*DST;
		Hash = K % INT32_MAX;
	}
	return Hash;
}
