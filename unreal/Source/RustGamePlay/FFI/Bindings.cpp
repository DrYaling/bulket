#include "RustApi.h"
#include "Binder.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerInput.h"
#include "Camera/CameraActor.h"
#include "Components/PrimitiveComponent.h"
#include "Sound/SoundBase.h"
#include "VisualLogger/VisualLogger.h"
#include "GameFramework/Actor.h"
#include "Containers/UnrealString.h"
#include "EngineUtils.h"
#include "../GameUnit/RustActor.h"
#include "../GameUnit/RustCharacter.h"
#include "../RustGameInstance.h"
#include "RustPlugin.h"
#include "../RustGameMode.h"
#include "../GameUnit/RustPawn.h"

#if ENABLE_DRAW_DEBUG

void DrawDebugSweptSphere(const UWorld* InWorld, FVector const& Start, FVector const& End, float Radius, FColor const& Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority = 0)
{
	FVector const TraceVec = End - Start;
	float const Dist = TraceVec.Size();

	FVector const Center = Start + TraceVec * 0.5f;
	float const HalfHeight = (Dist * 0.5f) + Radius;

	FQuat const CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	::DrawDebugCapsule(InWorld, Center, HalfHeight, Radius, CapsuleRot, Color, bPersistentLines, LifeTime, DepthPriority);
}

void DrawDebugSweptBox(const UWorld* InWorld, FVector const& Start, FVector const& End, FRotator const& Orientation, FVector const& HalfSize, FColor const& Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority = 0)
{
	FVector const TraceVec = End - Start;
	float const Dist = TraceVec.Size();

	FVector const Center = Start + TraceVec * 0.5f;

	FQuat const CapsuleRot = Orientation.Quaternion();
	::DrawDebugBox(InWorld, Start, HalfSize, CapsuleRot, Color, bPersistentLines, LifeTime, DepthPriority);

	//now draw lines from vertices
	FVector Vertices[8];
	Vertices[0] = Start + CapsuleRot.RotateVector(FVector(-HalfSize.X, -HalfSize.Y, -HalfSize.Z));	//flt
	Vertices[1] = Start + CapsuleRot.RotateVector(FVector(-HalfSize.X, HalfSize.Y, -HalfSize.Z));	//frt
	Vertices[2] = Start + CapsuleRot.RotateVector(FVector(-HalfSize.X, -HalfSize.Y, HalfSize.Z));	//flb
	Vertices[3] = Start + CapsuleRot.RotateVector(FVector(-HalfSize.X, HalfSize.Y, HalfSize.Z));	//frb
	Vertices[4] = Start + CapsuleRot.RotateVector(FVector(HalfSize.X, -HalfSize.Y, -HalfSize.Z));	//blt
	Vertices[5] = Start + CapsuleRot.RotateVector(FVector(HalfSize.X, HalfSize.Y, -HalfSize.Z));	//brt
	Vertices[6] = Start + CapsuleRot.RotateVector(FVector(HalfSize.X, -HalfSize.Y, HalfSize.Z));	//blb
	Vertices[7] = Start + CapsuleRot.RotateVector(FVector(HalfSize.X, HalfSize.Y, HalfSize.Z));		//brb
	for (int32 VertexIdx = 0; VertexIdx < 8; ++VertexIdx)
	{
		::DrawDebugLine(InWorld, Vertices[VertexIdx], Vertices[VertexIdx] + TraceVec, Color, bPersistentLines, LifeTime, DepthPriority);
	}

	::DrawDebugBox(InWorld, End, HalfSize, CapsuleRot, Color, bPersistentLines, LifeTime, DepthPriority);
}

/** Util for drawing result of single line trace  */
void DrawDebugLineTraceSingle(const UWorld* World, const FVector& Start, const FVector& End, EDrawDebugTrace::Type DrawDebugType, bool bHit, const FHitResult& OutHit, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		// @fixme, draw line with thickness = 2.f?
		if (bHit && OutHit.bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			::DrawDebugLine(World, Start, OutHit.ImpactPoint, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, OutHit.ImpactPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugPoint(World, OutHit.ImpactPoint, 16, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
}

/** Util for drawing result of multi line trace  */
void DrawDebugLineTraceMulti(const UWorld* World, const FVector& Start, const FVector& End, EDrawDebugTrace::Type DrawDebugType, bool bHit, const TArray<FHitResult>& OutHits, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		// @fixme, draw line with thickness = 2.f?
		if (bHit && OutHits.Last().bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			FVector const BlockingHitPoint = OutHits.Last().ImpactPoint;
			::DrawDebugLine(World, Start, BlockingHitPoint, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, BlockingHitPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}

		// draw hits
		for (int32 HitIdx = 0; HitIdx < OutHits.Num(); ++HitIdx)
		{
			FHitResult const& Hit = OutHits[HitIdx];
			::DrawDebugPoint(World, Hit.ImpactPoint, 16, (Hit.bBlockingHit ? TraceColor.ToFColor(true) : TraceHitColor.ToFColor(true)), bPersistent, LifeTime);
		}
	}
}

void DrawDebugBoxTraceSingle(const UWorld* World, const FVector& Start, const FVector& End, const FVector HalfSize, const FRotator Orientation, EDrawDebugTrace::Type DrawDebugType, bool bHit, const FHitResult& OutHit, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None && (World != nullptr))
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHit.bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			::DrawDebugSweptBox(World, Start, OutHit.Location, Orientation, HalfSize, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugSweptBox(World, OutHit.Location, End, Orientation, HalfSize, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugPoint(World, OutHit.ImpactPoint, 16, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugSweptBox(World, Start, End, Orientation, HalfSize, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
}

void DrawDebugBoxTraceMulti(const UWorld* World, const FVector& Start, const FVector& End, const FVector HalfSize, const FRotator Orientation, EDrawDebugTrace::Type DrawDebugType, bool bHit, const TArray<FHitResult>& OutHits, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None && (World != nullptr))
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHits.Last().bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			FVector const BlockingHitPoint = OutHits.Last().Location;
			::DrawDebugSweptBox(World, Start, BlockingHitPoint, Orientation, HalfSize, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugSweptBox(World, BlockingHitPoint, End, Orientation, HalfSize, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugSweptBox(World, Start, End, Orientation, HalfSize, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}

		// draw hits
		for (int32 HitIdx = 0; HitIdx < OutHits.Num(); ++HitIdx)
		{
			FHitResult const& Hit = OutHits[HitIdx];
			::DrawDebugPoint(World, Hit.ImpactPoint, 16, (Hit.bBlockingHit ? TraceColor.ToFColor(true) : TraceHitColor.ToFColor(true)), bPersistent, LifeTime);
		}
	}
}

void DrawDebugSphereTraceSingle(const UWorld* World, const FVector& Start, const FVector& End, float Radius, EDrawDebugTrace::Type DrawDebugType, bool bHit, const FHitResult& OutHit, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHit.bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			::DrawDebugSweptSphere(World, Start, OutHit.Location, Radius, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugSweptSphere(World, OutHit.Location, End, Radius, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugPoint(World, OutHit.ImpactPoint, 16, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugSweptSphere(World, Start, End, Radius, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
}

void DrawDebugSphereTraceMulti(const UWorld* World, const FVector& Start, const FVector& End, float Radius, EDrawDebugTrace::Type DrawDebugType, bool bHit, const TArray<FHitResult>& OutHits, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHits.Last().bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			FVector const BlockingHitPoint = OutHits.Last().Location;
			::DrawDebugSweptSphere(World, Start, BlockingHitPoint, Radius, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugSweptSphere(World, BlockingHitPoint, End, Radius, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugSweptSphere(World, Start, End, Radius, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}

		// draw hits
		for (int32 HitIdx = 0; HitIdx < OutHits.Num(); ++HitIdx)
		{
			FHitResult const& Hit = OutHits[HitIdx];
			::DrawDebugPoint(World, Hit.ImpactPoint, 16, (Hit.bBlockingHit ? TraceColor.ToFColor(true) : TraceHitColor.ToFColor(true)), bPersistent, LifeTime);
		}
	}
}

void DrawDebugCapsuleTraceSingle(const UWorld* World, const FVector& Start, const FVector& End, float Radius, float HalfHeight, EDrawDebugTrace::Type DrawDebugType, bool bHit, const FHitResult& OutHit, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHit.bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			::DrawDebugCapsule(World, Start, HalfHeight, Radius, FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugCapsule(World, OutHit.Location, HalfHeight, Radius, FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, Start, OutHit.Location, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugPoint(World, OutHit.ImpactPoint, 16, TraceColor.ToFColor(true), bPersistent, LifeTime);

			::DrawDebugCapsule(World, End, HalfHeight, Radius, FQuat::Identity, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, OutHit.Location, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugCapsule(World, Start, HalfHeight, Radius, FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugCapsule(World, End, HalfHeight, Radius, FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
}

void DrawDebugCapsuleTraceMulti(const UWorld* World, const FVector& Start, const FVector& End, float Radius, float HalfHeight, EDrawDebugTrace::Type DrawDebugType, bool bHit, const TArray<FHitResult>& OutHits, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHits.Last().bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			FVector const BlockingHitPoint = OutHits.Last().Location;
			::DrawDebugCapsule(World, Start, HalfHeight, Radius, FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugCapsule(World, BlockingHitPoint, HalfHeight, Radius, FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, Start, BlockingHitPoint, TraceColor.ToFColor(true), bPersistent, LifeTime);

			::DrawDebugCapsule(World, End, HalfHeight, Radius, FQuat::Identity, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, BlockingHitPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugCapsule(World, Start, HalfHeight, Radius, FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugCapsule(World, End, HalfHeight, Radius, FQuat::Identity, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}

		// draw hits
		for (int32 HitIdx = 0; HitIdx < OutHits.Num(); ++HitIdx)
		{
			FHitResult const& Hit = OutHits[HitIdx];
			::DrawDebugPoint(World, Hit.ImpactPoint, 16, (Hit.bBlockingHit ? TraceColor.ToFColor(true) : TraceHitColor.ToFColor(true)), bPersistent, LifeTime);
		}
	}
}

#endif // ENABLE_DRAW_DEBUG

void SetSpatialData(AActorOpaque* actor,
                    const Vector3 position,
                    const Quaternion rotation,
                    const Vector3 scale)
{
	ToAActor(actor)->SetActorTransform(FTransform(ToFQuat(rotation), ToFVector(position), ToFVector(scale)));
}

void TickActor(const AActorOpaque* actor, float dt)
{
	ToAActor(actor)->Tick(dt);
}

void GetSpatialData(const AActorOpaque* actor,
                    Vector3* position,
                    Quaternion* rotation,
                    Vector3* scale)
{
	const auto Transform = ToAActor(actor)->GetTransform();/*
	*position = ToVector3(Transform.GetTranslation());
	*rotation = ToQuaternion(Transform.GetRotation());
	*scale = ToVector3(Transform.GetScale3D());*/
}

void Log(const char* s, int32_t len, int32_t logLevel)
{
	// TODO: Can we get rid of that allocation?
	FString LogString = FString(len, UTF8_TO_TCHAR(s));
	const RustApiLogLevel level = static_cast<RustApiLogLevel>(logLevel);
	switch (level)
	{
		case Error:
			UE_LOG(LogNative, Error, TEXT("%s"), *LogString);
			break;
		case Warn:
			UE_LOG(LogNative, Warning, TEXT("%s"), *LogString);
			break;
		case Info:
			UE_LOG(LogNative, Display, TEXT("%s"), *LogString);
			break;
		case Trace:
			UE_LOG(LogNative, Log, TEXT("%s"), *LogString);
			break;
	}
}

void IterateActors(AActorOpaque** array, uint64_t* len)
{

	uint64_t i = 0;
	//for (TActorIterator<ARustActor> ActorItr(GetGameMode()->GetWorld()); ActorItr; ++ActorItr, ++i)
	//{
	//	if (i >= *len)
	//		return;
	//	AActorOpaque* a = (AActorOpaque*)*ActorItr;
	//	array[i] = a;
	//}
	*len = i;
}

void GetActionState(const char* name, uintptr_t len, ActionState state, uint32_t* out)
{
	//APlayerController* PC = UGameplayStatics::GetPlayerController(GetGameMode(), 0);

	//FName ActionName((int32)len, name);

	//for (auto M : PC->PlayerInput->GetKeysForAction(ActionName))
	//{
	//	if (state == ActionState::Pressed)
	//	{
	//		if (PC->PlayerInput->WasJustPressed(M.Key))
	//		{
	//			*out = true;
	//			return;
	//		}
	//	}
	//	if (state == ActionState::Released)
	//	{
	//		if (PC->PlayerInput->WasJustReleased(M.Key))
	//		{
	//			*out = true;
	//			return;
	//		}
	//	}
	//	if (state == ActionState::Held)
	//	{
	//		if (PC->PlayerInput->IsPressed(M.Key))
	//		{
	//			*out = true;
	//			return;
	//		}
	//	}
	//}
	*out = false;
}

void GetAxisValue(const char* name, uintptr_t len, float* value)
{
}

void SetEntityForActor(AActorOpaque* actor, Entity entity)
{
	//ARustActor* RustActor = Cast<ARustActor>(ToAActor(actor));
	//if (RustActor != nullptr && RustActor->EntityComponent != nullptr)
	//{
	//	RustActor->EntityComponent->Id.Id = entity.id;
	//}
	//else
	//{
	//	UEntityComponent* Component = NewObject<UEntityComponent>(ToAActor(actor), TEXT("EntityComponent"));
	//	Component->Id.Id = entity.id;

	//	Component->CreationMethod = EComponentCreationMethod::Native;
	//	ToAActor(actor)->AddOwnedComponent(Component);
	//	Component->RegisterComponent();
	//}
}

AActorOpaque* SpawnActor(ActorClass class_,
                         Vector3 position,
                         Quaternion rotation,
                         Vector3 scale)
{
	/*for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;

		FName Name = Class->ClassConfigName;
		if (Cast<ARustActor>(Class->GetDefaultObject(false)) != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Class %s"), *Class->GetDesc());
		}
	}*/

	UClass* Class;
	switch (class_)
	{
	case ActorClass::CameraActor:
		Class = ACameraActor::StaticClass();
		break;
	case ActorClass::RustActor:
		Class = ARustActor::StaticClass();
		break;
	case ActorClass::RustCharacter:
		Class = ARustCharacter::StaticClass();
		break;
	case ActorClass::RustPawn:
		Class = ARustPawn::StaticClass();
		break;
	default:
		// :(
		Class = ARustActor::StaticClass();
	};
	FVector Pos = ToFVector(position);
	FRotator Rot = ToFQuat(rotation).Rotator();
	return (AActorOpaque*)GetGameMode()->GetWorld()->SpawnActor(Class, &Pos, &Rot, FActorSpawnParameters{});
}

AActorOpaque* SpawnActorBP(
	NativeString BPname, 
	Transform transform,
	ActorClass actorType)
{
	const auto SGame = sGameInstance;
	if (!SGame)
		return nullptr;
	const FString FbpName = Utf82FString(BPname);
	switch (actorType)
	{
	case ActorClass::RustCharacter:
		return SGame->SpawnCharacter(FbpName, ToFTransform(transform), TEXT("RustCharacter from rust"));
	case ActorClass::RustPawn:
		return SGame->SpawnCreature(FbpName, ToFTransform(transform), TEXT("RustPawn from rust"));
	case ActorClass::RustActor:
		return SGame->SpawnWorldObject(FbpName, ToFTransform(transform), TEXT("RustActor from rust"));
	default:
		UE_LOG(LogNative, Error, TEXT("Undefined spaner %s %d"), *FbpName, actorType);
		return nullptr;
	}
}

void SetViewTarget(const AActorOpaque* actor)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetGameMode(ToAActor(actor)->GetWorld()), 0);
	PC->SetViewTarget(ToAActor(actor), FViewTargetTransitionParams());
}

void GetMouseDelta(float* x, float* y)
{
	/*APlayerController* PC = UGameplayStatics::GetPlayerController(GetGameMode(), 0);
	PC->GetInputMouseDelta(*x, *y);*/
}

void GetActorComponents(const AActorOpaque* actor, ActorComponentPtr* data, uintptr_t* len)
{
	TSet<UActorComponent*> Components = ToAActor(actor)->GetComponents();
	if (data == nullptr)
	{
		*len = Components.Num();
	}
	else
	{
		uintptr_t MaxComponentNum = *len;
		uintptr_t i = 0;
		for (auto& Component : Components)
		{
			if (i == MaxComponentNum)
			{
				break;
			}
			if (Cast<UPrimitiveComponent>(Component) != nullptr)
			{
				data[i] =
					ActorComponentPtr{
						ActorComponentType::Primitive,
						(void*)Component
					};

				i += 1;
			}
		}
		*len = i;
	}
}

void AddForce(UPrimtiveOpaque* actor, Vector3 force)
{
	static_cast<UPrimitiveComponent*>(actor)->AddForce(ToFVector(force), FName{}, false);
}

void AddImpulse(UPrimtiveOpaque* actor, Vector3 force)
{
	static_cast<UPrimitiveComponent*>(actor)->AddImpulse(ToFVector(force), FName{}, false);
}

uint32_t IsSimulating(const UPrimtiveOpaque* primitive)
{
	return ((UPrimitiveComponent*)primitive)->IsSimulatingPhysics(FName{});
}

Vector3 GetVelocity(const UPrimtiveOpaque* primitive)
{
	return ToVector3(((UPrimitiveComponent*)primitive)->GetComponentVelocity());
}

void SetVelocity(UPrimtiveOpaque* primitive, Vector3 velocity)
{
	((UPrimitiveComponent*)primitive)->SetPhysicsLinearVelocity(ToFVector(velocity), false, FName{});
}

uint32_t LineTrace(Vector3 start, Vector3 end, LineTraceParams Params, HitResult* result)
{
	FHitResult Out;
	auto CollisionParams = FCollisionQueryParams();
	for (uintptr_t i = 0; i < Params.ignored_actors_len; ++i)
	{
		CollisionParams.AddIgnoredActor((AActor*)Params.ignored_actors[i]);
	}
	auto GameMode = GetGameMode();
	if (GameMode == nullptr) {
		UE_LOG(LogNative, Error, TEXT("RustNative should run under ARustGameMode"));
		return 0;
	}

	//bool IsHit = UKismetSystemLibrary::LineTraceSingle(
	//	GameMode->GetWorld(),
	//	location, forward * 1000 + location, TraceTypeQuery1, false,
	//	ignores, EDrawDebugTrace::Type::ForOneFrame, result,
	//	true));
	auto World = GameMode->GetWorld();
	const auto Start = ToFVector(start);
	const auto End = ToFVector(end);
	bool IsHit = World->LineTraceSingleByChannel(
		Out, Start, End, Params.channel, CollisionParams, FCollisionResponseParams{});

#if ENABLE_DRAW_DEBUG
	if (Params.trace_debug)
	{
		DrawDebugLineTraceSingle(World, Start, End, EDrawDebugTrace::ForDuration, IsHit, Out, FLinearColor::Red, FLinearColor::Green, 0.5f);
	}
#endif
	if (IsHit)
	{
		result->actor = (AActorOpaque*)Out.GetActor();
		result->primtive = (UPrimtiveOpaque*)Out.GetComponent();
		result->distance = Out.Distance;
		result->location = ToVector3(Out.Location);
		result->normal = ToVector3(Out.Normal);
		result->impact_location = ToVector3(Out.ImpactPoint);
		result->pentration_depth = Out.PenetrationDepth;
		result->hit_bone = ToUName(Out.BoneName);
		if (const auto HitUnit = Cast<IRustObjectInterface>(Out.GetActor()))
		{
			result->unit_uuid = HitUnit->GetUnitState().Uuid;
			result->unit_type = HitUnit->GetUnitState().UnitType;

		}
		else
		{
			result->unit_type = ERustUnitType::Undefined;
		}
	}

	return IsHit ? 1: 0;
}

uint32_t OverlapMulti(CollisionShape shape,
                      Vector3 position,
                      Quaternion rotation,
                      LineTraceParams params,
                      uintptr_t max_results,
                      OverlapResult** results)
{
	TArray<FOverlapResult> Out;
	auto CollisionParams = FCollisionQueryParams();
	auto GameMode = GetGameMode();
	if (GameMode == nullptr) {
		UE_LOG(LogNative, Error, TEXT("RustNative should run under ARustGameMode"));
		return 0;
	}
	for (uintptr_t i = 0; i < params.ignored_actors_len; ++i)
	{
		CollisionParams.AddIgnoredActor((AActor*)params.ignored_actors[i]);
	}
	bool IsHit = GameMode->GetWorld()->OverlapMultiByChannel(Out,
	                                                                         ToFVector(position),
	                                                                         ToFQuat(rotation),
	                                                                         ECollisionChannel::ECC_MAX,
	                                                                         ToFCollisionShape(shape),
	                                                                         CollisionParams,
	                                                                         FCollisionResponseParams{});
	if (IsHit)
	{
		uintptr_t Length = FGenericPlatformMath::Min(max_results, (uintptr_t)Out.Num());
		for (uintptr_t i = 0; i < Length; ++i)
		{
			OverlapResult* result = results[i];
			FOverlapResult* Hit = &Out[i];
			result->actor = (AActorOpaque*)Hit->GetActor();
			result->primtive = (UPrimtiveOpaque*)Hit->GetComponent();
		}
	}

	return IsHit;
}

void VisualLogSegment(const AActorOpaque* actor, Vector3 start, Vector3 end, Color color)
{
	UE_VLOG_SEGMENT(ToAActor(actor), LogNative, Log, ToFVector(start), ToFVector(end), ToFColor(color), TEXT(""));
}

void VisualLogCapsule(
	Utf8Str category,
	const AActorOpaque* owner,
	Vector3 position,
	Quaternion rotation,
	float half_height,
	float radius,
	Color color)
{
	FString CategoryStr = ToFString(category);
	auto LogCat = FLogCategory<ELogVerbosity::Log, ELogVerbosity::All>(*CategoryStr);
	FVector Base = ToFVector(position) - half_height * (ToFQuat(rotation) * FVector::UpVector);
	UE_VLOG_CAPSULE(ToAActor(owner), LogCat, Log, Base, half_height, radius, ToFQuat(rotation),
	                ToFColor(color), TEXT(""));
}

void GetRootComponent(const AActorOpaque* actor, ActorComponentPtr* data)
{
	USceneComponent* Root = ToAActor(actor)->GetRootComponent();

	if (Cast<UPrimitiveComponent>(Root) != nullptr)
	{
		*data = ActorComponentPtr{ActorComponentType::Primitive, (void*)Root};
		return;
	}
}

Vector3 GetBoundingBoxExtent(const UPrimtiveOpaque* primitive)
{
	return ToVector3(((UPrimitiveComponent*)primitive)->Bounds.BoxExtent);
}

uint32_t Sweep(Vector3 start,
               Vector3 end,
               Quaternion rotation,
               LineTraceParams params,
               CollisionShape shape,
               HitResult* result)
{
	FHitResult Out;
	auto CollisionParams = FCollisionQueryParams();
	for (uintptr_t i = 0; i < params.ignored_actors_len; ++i)
	{
		CollisionParams.AddIgnoredActor((AActor*)params.ignored_actors[i]);
		CollisionParams.bFindInitialOverlaps = true;
		// TODO: Make configurable
		CollisionParams.bDebugQuery = true;
	}
	const auto Start = ToFVector(start);
	const auto End = ToFVector(end);
	const auto Rotation = ToFQuat(rotation);
	const auto World = GetGameMode()->GetWorld();
	bool IsHit = World->SweepSingleByChannel(
		Out,
		Start,
		End,
		Rotation,
		params.channel,
		ToFCollisionShape(shape),
		CollisionParams, FCollisionResponseParams{});
#if ENABLE_DRAW_DEBUG
	if (params.trace_debug){
		switch (shape.ty)
		{
		case CollisionShapeType::Box:
			{
				FVector HalfSize = FVector(shape.data.collision_box.half_extent_x, shape.data.collision_box.half_extent_y, shape.data.collision_box.half_extent_z);
				DrawDebugBoxTraceSingle(
					World, Start, End,
					HalfSize,
					Rotation.Rotator(),
					EDrawDebugTrace::ForDuration,
					IsHit, Out, FColor::Blue,
					FColor::Green, 1.0
				);
			}
			break;
		case CollisionShapeType::Capsule:
			DrawDebugCapsuleTraceSingle(
				World, Start, End,
				shape.data.capsule.radius,
				shape.data.capsule.half_height,
				EDrawDebugTrace::ForDuration,
				IsHit, Out, FColor::Blue,
				FColor::Green, 1.0
			);
			break;
		case CollisionShapeType::Sphere:
			DrawDebugSphereTraceSingle(
				World, Start, End,
				shape.data.sphere.radius,
				EDrawDebugTrace::ForDuration,
				IsHit, Out, FColor::Blue,
				FColor::Green, 1.0
			);
			break;
		default: ;
		}
	}
#endif
	if (IsHit)
	{
		result->actor = (AActorOpaque*)Out.GetActor();
		result->primtive = Out.GetComponent();
		result->distance = Out.Distance;
		result->location = ToVector3(Out.Location);
		result->normal = ToVector3(Out.Normal);
		result->impact_location = ToVector3(Out.ImpactPoint);
		result->impact_normal = ToVector3(Out.ImpactNormal);
		result->pentration_depth = Out.PenetrationDepth;
		result->start_penetrating = Out.bStartPenetrating;
		result->hit_bone = ToUName(Out.BoneName);
		if (const auto HitUnit = Cast<IRustObjectInterface>(Out.GetActor()))
		{
			result->unit_uuid = HitUnit->GetUnitState().Uuid;
			result->unit_type = HitUnit->GetUnitState().UnitType;

		}
		else
		{
			result->unit_type = ERustUnitType::Undefined;
		}
	}

	return IsHit;
}

uint32_t SweepMulti(Vector3 start,
                    Vector3 end,
                    Quaternion rotation,
                    LineTraceParams params,
                    CollisionShape shape,
                    uintptr_t max_results,
                    HitResult* results)
{
	TArray<FHitResult> Out;
	auto GameMode = GetGameMode();
	if (GameMode == nullptr) {
		UE_LOG(LogNative, Error, TEXT("RustNative should run under ARustGameMode"));
		return 0;
	}

	auto CollisionParams = FCollisionQueryParams();
	for (uintptr_t i = 0; i < params.ignored_actors_len; ++i)
	{
		CollisionParams.AddIgnoredActor((AActor*)params.ignored_actors[i]);
		CollisionParams.bFindInitialOverlaps = true;
		// TODO: Make configurable
		CollisionParams.bDebugQuery = true;
	}
	const auto Start = ToFVector(start);
	const auto End = ToFVector(end);
	const auto Rotation = ToFQuat(rotation);
	const auto World = GetGameMode()->GetWorld();
	bool IsHit = World->SweepMultiByChannel(
		Out,
		ToFVector(start),
		ToFVector(end),
		ToFQuat(rotation),
		params.channel,
		ToFCollisionShape(shape),
		CollisionParams, FCollisionResponseParams{});

#if ENABLE_DRAW_DEBUG
	if (params.trace_debug) {
		switch (shape.ty)
		{
		case CollisionShapeType::Box:
			{
				FVector HalfSize = FVector(shape.data.collision_box.half_extent_x, shape.data.collision_box.half_extent_y, shape.data.collision_box.half_extent_z);
				DrawDebugBoxTraceMulti(
					World, Start, End,
					HalfSize,
					Rotation.Rotator(),
					EDrawDebugTrace::ForDuration,
					IsHit, Out, FColor::Blue,
					FColor::Green, 1.0
				);
			}
			break;
		case CollisionShapeType::Capsule:
			DrawDebugCapsuleTraceMulti(
				World, Start, End,
				shape.data.capsule.radius,
				shape.data.capsule.half_height,
				EDrawDebugTrace::ForDuration,
				IsHit, Out, FColor::Blue,
				FColor::Green, 1.0
			);
			break;
		case CollisionShapeType::Sphere:
			DrawDebugSphereTraceMulti(
				World, Start, End,
				shape.data.sphere.radius,
				EDrawDebugTrace::ForDuration,
				IsHit, Out, FColor::Blue,
				FColor::Green, 1.0
			);
			break;
		default:;
		}
	}
#endif
	uintptr_t Length = FGenericPlatformMath::Min(max_results, (uintptr_t)Out.Num());
	if (IsHit)
	{
		for (uintptr_t i = 0; i < Length; ++i)
		{
			FHitResult& Hit = Out[i];

			results[i].actor = (AActorOpaque*)Hit.GetActor();
			results[i].distance = Hit.Distance;
			results[i].location = ToVector3(Hit.Location);
			results[i].normal = ToVector3(Hit.Normal);
			results[i].impact_location = ToVector3(Hit.ImpactPoint);
			results[i].impact_normal = ToVector3(Hit.ImpactNormal);
			results[i].pentration_depth = Hit.PenetrationDepth;
			results[i].start_penetrating = Hit.bStartPenetrating;
			results[i].hit_bone = ToUName(Hit.BoneName);
			if(const auto HitUnit = Cast<IRustObjectInterface>(Hit.GetActor()))
			{
				results[i].unit_uuid = HitUnit->GetUnitState().Uuid;
				results[i].unit_type = HitUnit->GetUnitState().UnitType;

			}
			else
			{
				results[i].unit_type = ERustUnitType::Undefined;
			}
		}
	}
	return Length;
}

void GetRegisteredClasses(UClassOpague** classes, uintptr_t* len)
{
	/*if (classes == nullptr)
	{
		*len = sGameInstance->RegisteredClasses.Num();
		return;
	}
	uintptr_t Count = *len;
	for (uintptr_t Idx = 0; Idx < Count; ++Idx)
	{
		classes[Idx] = (UClassOpague*)sGameInstance ->RegisteredClasses[Idx].Get();
	}*/
}

UClassOpague* GetClass(const AActorOpaque* actor)
{
	return (UClassOpague*)ToAActor(actor)->GetClass();
}

uint32_t IsMoveable(const AActorOpaque* actor)
{
	return ToAActor(actor)->IsRootComponentMovable();
}

void SetOwner(AActorOpaque* actor, const AActorOpaque* new_owner)
{
	ToAActor(actor)->SetOwner(ToAActor(new_owner));
}

uint32_t GetCollisionShape(const UPrimtiveOpaque* primitive, CollisionShape* out)
{
	const FCollisionShape UnrealShape = static_cast<const UPrimitiveComponent*>(primitive)->GetCollisionShape();


	if (UnrealShape.IsBox())
	{
		CollisionShape Shape;
		Shape.ty = CollisionShapeType::Box;
		const FVector HalfExtent = UnrealShape.GetBox();
		CollisionBox Box;
		Box.half_extent_x = HalfExtent.X;
		Box.half_extent_y = HalfExtent.Y;
		Box.half_extent_z = HalfExtent.Z;
		Shape.data.collision_box = Box;

		*out = Shape;
		return 1;
	}

	if (UnrealShape.IsSphere())
	{
		CollisionShape Shape;
		Shape.ty = CollisionShapeType::Sphere;
		const float Radius = UnrealShape.GetSphereRadius();
		CollisionSphere Sphere;

		Sphere.radius = Radius;
		Shape.data.sphere = Sphere;

		*out = Shape;
		return 1;
	}

	if (UnrealShape.IsCapsule())
	{
		CollisionShape Shape;
		Shape.ty = CollisionShapeType::Capsule;
		const float Radius = UnrealShape.GetCapsuleRadius();
		const float HalfHeight = UnrealShape.GetCapsuleHalfHeight();
		CollisionCapsule Capsule;
		Capsule.radius = Radius;
		Capsule.half_height = HalfHeight;
		Shape.data.capsule = Capsule;

		*out = Shape;
		return 1;
	}
	// TODO: Handle line instead?
	return 0;
}

void VisualLogLocation(Utf8Str category, const AActorOpaque* owner, Vector3 position, float radius, Color color)
{
	FString CategoryStr = ToFString(category);
	auto LogCat = FLogCategory<ELogVerbosity::Log, ELogVerbosity::All>(*CategoryStr);
	UE_VLOG_LOCATION(ToAActor(owner), LogCat, Log, ToFVector(position), radius, ToFColor(color), TEXT(""));
}

uint32_t GetEditorComponentUuids(const AActorOpaque* actor, Uuid* data, uintptr_t* len)
{
	AActor* Actor = ToAActor(actor);
	if (Actor == nullptr)
	{
		return 0;
	}
	//UEntityComponent* EntityComponent = Actor->FindComponentByClass<UEntityComponent>();
	//if (EntityComponent == nullptr)
	//{
	//	return 0;
	//}

	//if (data == nullptr)
	//{
	//	*len = EntityComponent->Components.Num();
	//	return 1;
	//}
	//uintptr_t Count = *len;
	//uintptr_t Idx = 0;
	//for (auto& Elem : EntityComponent->Components)
	//{
	//	if (Idx == Count)
	//		return 1;
	//	FGuid Guid;
	//	FGuid::Parse(Elem.Key, Guid);
	//	data[Idx] = ToUuid(Guid);
	//	Idx += 1;
	//}

	//// We have only partially written to data
	//*len = Idx;
	return 1;
}

uint32_t GetEditorComponentVector(const AActorOpaque* actor, Uuid uuid, Utf8Str field, Vector3* out)
{
	/*FRustProperty* Prop = GetRustProperty(actor, uuid, field);
	if (Prop == nullptr)
		return 0;

	if (Prop->Tag != ERustPropertyTag::Vector)
		return 0;

	*out = ToVector3(Prop->Vector);*/
	return 1;
}

uint32_t GetEditorComponentFloat(const AActorOpaque* actor, Uuid uuid, Utf8Str field, float* out)
{
	/*FRustProperty* Prop = GetRustProperty(actor, uuid, field);
	if (Prop == nullptr)
		return 0;

	if (Prop->Tag != ERustPropertyTag::Float)
		return 0;

	*out = Prop->Float;*/
	return 1;
}

uint32_t GetEditorComponentBool(const AActorOpaque* actor, Uuid uuid, Utf8Str field, uint32_t* out)
{
	/*FRustProperty* Prop = GetRustProperty(actor, uuid, field);
	if (Prop == nullptr)
		return 0;

	if (Prop->Tag != ERustPropertyTag::Bool)
		return 0;

	*out = Prop->Bool == 1;*/
	return 1;
}

uint32_t GetEditorComponentQuat(const AActorOpaque* actor, Uuid uuid, Utf8Str field, Quaternion* out)
{
	/*FRustProperty* Prop = GetRustProperty(actor, uuid, field);
	if (Prop == nullptr)
		return 0;

	if (Prop->Tag != ERustPropertyTag::Quat)
		return 0;

	*out = ToQuaternion(Prop->Rotation.Quaternion());*/
	return 1;
}

uint32_t GetEditorComponentUObject(const AActorOpaque* actor, Uuid uuid, Utf8Str field, UObjectType ty,
                                   UObjectOpague** out)
{
	/*FRustProperty* Prop = GetRustProperty(actor, uuid, field);
	if (Prop == nullptr)
		return 0;

	if (Prop->Tag == ERustPropertyTag::Class)
	{
		*out = static_cast<UObjectOpague*>(Prop->Class.Get());
		return 1;
	}
	if (Prop->Tag == ERustPropertyTag::Sound)
	{
		*out = static_cast<UObjectOpague*>(Prop->Sound.Get());
		return 1;
	}*/

	return 0;
}

void PlaySoundAtLocation(const USoundBaseOpague* sound, Vector3 location, Quaternion rotation,
                         const SoundSettings* settings)
{
	auto World = GetGameMode()->GetWorld();
	UGameplayStatics::PlaySoundAtLocation(World, (USoundBase*)sound, ToFVector(location), ToFQuat(rotation).Rotator());
}

void GetActorName(const AActorOpaque* actor, RustAlloc* data)
{
	FString Name = ToAActor(actor)->GetActorNameOrLabel();/*
	auto Utf8 = FTCHARToUTF8(*Name);
	GetRustModule().Plugin.Rust.allocate_fns.allocate(Utf8.Length(), 1, data);
	FMemory::Memcpy(data->ptr, Utf8.Get(), data->size);*/
}

void RegisterActorOnOverlap(AActorOpaque* actor)
{
	auto GameMode = GetGameMode();
	AActor* Actor = ToAActor(actor);
	if (!GameMode || !Actor)
		return;
	Actor->OnActorBeginOverlap.AddUniqueDynamic(GameMode, &ARustGameMode::OnActorBeginOverlap);
	Actor->OnActorEndOverlap.AddUniqueDynamic(GameMode, &ARustGameMode::OnActorEndOverlap);
}

void RegisterActorOnHit(AActorOpaque* actor)
{
	auto GameMode = GetGameMode();
	AActor* Actor = ToAActor(actor);
	if (!GameMode || !Actor)
		return;
	Actor->OnActorHit.AddUniqueDynamic(GameMode, &ARustGameMode::OnActorHit);
}

void DestroyActor(const AActorOpaque* actor)
{
	// TODO: What do we do if we can't destroy the actor?
	ToAActor(actor)->Destroy();
}
bool SetActorLocationAndRotation(
	const AActorOpaque* Actor, 
	Vector3 LocationOrRotation, 
	bool SetLocation,
	HitResult* result,
	ETeleportType Teleport
){
	if(SetLocation)
	{
		FVector Vector = ToFVector(LocationOrRotation);
		bool moveResult;
		if (result != nullptr)
		{
			FHitResult Out;
			moveResult = ToAActor(Actor)->SetActorLocation(Vector, true, &Out, Teleport);
			if (result)
			{
				result->actor = (AActorOpaque*)Out.GetActor();
				result->primtive = Out.GetComponent();
				result->distance = Out.Distance;
				result->location = ToVector3(Out.Location);
				result->normal = ToVector3(Out.Normal);
				result->impact_location = ToVector3(Out.ImpactPoint);
				result->impact_normal = ToVector3(Out.ImpactNormal);
				result->pentration_depth = Out.PenetrationDepth;
				result->start_penetrating = Out.bStartPenetrating;
				result->hit_bone = ToUName(Out.BoneName);
				if (const auto HitUnit = Cast<IRustObjectInterface>(Out.GetActor()))
				{
					result->unit_uuid = HitUnit->GetUnitState().Uuid;
					result->unit_type = HitUnit->GetUnitState().UnitType;

				}
				else
				{
					result->unit_type = ERustUnitType::Undefined;
				}

			}
		}
		else
			moveResult = ToAActor(Actor)->SetActorLocation(Vector, false, nullptr, Teleport);
		return moveResult;
	}
	else
	{
		return ToAActor(Actor)->SetActorRotation(FRotator(LocationOrRotation.x, LocationOrRotation.y, LocationOrRotation.z), Teleport);
	}
}