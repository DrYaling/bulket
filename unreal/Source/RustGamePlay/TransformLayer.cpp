#include "TransformLayer.h"
#include "Animation/AnimSequence.h"
#include "GameUnit/RustActor.h"
#include "RustGameInstance.h"
#include <Animation/AnimSequenceHelpers.h>
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

ERustUnitType UTransformLayer::GetUnitType(int32 UUID)
{
	if (UUID == 0)
		return ERustUnitType::Undefined;
	if (UUID < 0)
		return ERustUnitType::Creature;
	if ((UUID >> 30) > 0)
		return ERustUnitType::WorldObject;
	return ERustUnitType::Character;
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

UAnimSequence* UTransformLayer::AnimationClip(UAnimSequence* Anim, int StartFrame, int EndFrame)
{
	const float StartTime = Anim->GetTimeAtFrame(StartFrame);// Anim->GetPlayLength() / (float)(Anim->GetNumberOfFrames() - 1) * (float)EndFrame;
	const float EndTime = Anim->GetTimeAtFrame(EndFrame);// Anim->GetPlayLength() / (float)(Anim->GetNumberOfFrames() - 1) * (float)StartKeyIndex;
	int frames = Anim->GetNumberOfSampledKeys();
	float dura = Anim->GetPlayLength();
	UE::Anim::AnimationData::Trim(Anim, EndTime, Anim->GetPlayLength(), true);
	if(StartFrame > 0)
		UE::Anim::AnimationData::Trim(Anim, 0, StartTime, false);

	int framesNew = Anim->GetNumberOfSampledKeys();
	float duraNew = Anim->GetPlayLength();
	UE_LOG(LogNative, Display, TEXT("anim -> %d(%f) : %d(%f)"), frames, dura, framesNew, duraNew);
	return Anim;
}
