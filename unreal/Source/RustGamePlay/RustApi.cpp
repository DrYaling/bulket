
#include "RustApi.h"

#include "Binder.h"
#include "RustPlugin.h"
#include "CoreTypes.h"
#include "Logging/LogMacros.h"
#include "Kismet/GameplayStatics.h"
#include "RustGameMode.h"
#include "RustGameInstance.h"
DEFINE_LOG_CATEGORY(LogNative);

//FRustPluginModule& GetModule()
//{
//	return FModuleManager::LoadModuleChecked<FRustPluginModule>(TEXT("ProjectU"));
//}

// #define LogNative LogTemp
const FPlugin* GetRustModule(const UObject* world) {
	return sGameInstance->GetPlugin();
}

ARustGameMode* GetGameMode(const UObject* world) {
	return sGameInstance->GameMode();
}


FColor ToFColor(Color c)
{
	return FColor(c.r, c.g, c.b, c.a);
}

CollisionShape ToCollisionShape(FCollisionShape Shape)
{
	CollisionShape ret;
	if(Shape.IsCapsule())
	{
		ret.ty = CollisionShapeType::Capsule;
		ret.data.capsule.half_height = Shape.Capsule.HalfHeight;
		ret.data.capsule.radius = Shape.Capsule.Radius;
	}
	else if(Shape.IsSphere())
	{
		ret.ty = CollisionShapeType::Sphere;
		ret.data.sphere.radius = Shape.Sphere.Radius;
	}//box
	else
	{
		ret.ty = CollisionShapeType::Box;
		ret.data.collision_box.half_extent_x = Shape.Box.HalfExtentX;
		ret.data.collision_box.half_extent_y = Shape.Box.HalfExtentY;
		ret.data.collision_box.half_extent_z = Shape.Box.HalfExtentZ;
	}
	return ret;
}
FCollisionShape ToFCollisionShape(CollisionShape Shape)
{
	if (Shape.ty == CollisionShapeType::Box)
	{
		return FCollisionShape::MakeBox(FVector3f(
			Shape.data.collision_box.half_extent_x,
			Shape.data.collision_box.half_extent_y,
			Shape.data.collision_box.half_extent_z
		));
	}
	if (Shape.ty == CollisionShapeType::Sphere)
	{
		return FCollisionShape::MakeSphere(Shape.data.sphere.radius);
	}

	if (Shape.ty == CollisionShapeType::Capsule)
	{
		return FCollisionShape::MakeCapsule(
			Shape.data.capsule.radius,
			Shape.data.capsule.half_height
		);
	}

	// TODO: Unreal way?
	abort();
}

FString ToFString(Utf8Str Str)
{
	if (Str.len == 0)
		return FString();

	return FString(Str.len, UTF8_TO_TCHAR(Str.ptr));
}

Quaternion ToQuaternion(FQuat q)
{
	Quaternion r;
	r.x = q.X;
	r.y = q.Y;
	r.z = q.Z;
	r.w = q.W;
	return r;
}

Vector3 ToVector3(FVector v)
{
	Vector3 r;
	r.x = v.X;
	r.y = v.Y;
	r.z = v.Z;
	return r;
}

FVector ToFVector(Vector3 v)
{
	return FVector(v.x, v.y, v.z);
}

FQuat ToFQuat(Quaternion q)
{
	return FQuat(q.x, q.y, q.z, q.w);
}
FIntPoint ToFIntPoint(IntPoint q)
{
	return FIntPoint(q.x, q.y);
}
IntPoint ToFIntPoint(FIntPoint q)
{
	struct IntPoint r;
	r.x = q.X;
	r.y = q.Y;
	return r;
}
Transform ToTransform(FTransform transform)
{
	Transform r;
	r.Location = ToVector3(transform.GetLocation());
	r.Scale = ToVector3(transform.GetScale3D());
	r.Rotation = ToQuaternion(transform.GetRotation());
	return r;
}
FTransform ToFTransform(Transform transform)
{
	return FTransform(ToFQuat(transform.Rotation),  ToFVector(transform.Location), ToFVector(transform.Scale));
}

FRotator ToFRotator(Rotator r)
{
	return FRotator(r.Pitch, r.Yaw, r.Roll);
}
FPlane ToFPlane(Vector4 v)
{
	return FPlane(v.x, v.y, v.z, v.w);
}
Vector4 FromFPlane(FPlane v)
{
	Vector4 v4;
	v4.x = v.X;
	v4.y = v.Y;
	v4.z = v.Z;
	v4.w = v.W;
	return v4;
}
Rotator ToRotator(FRotator r)
{
	Rotator rt;
	rt.Pitch = r.Pitch;
	rt.Yaw = r.Yaw;
	rt.Roll = r.Roll;
	return rt;
}

FVector4 ToFVector4(Vector4 v)
{
	return FVector4(v.x, v.y, v.z, v.w);
}
Vector4 ToVector4(FVector4 v)
{
	Vector4 v4;
	v4.x = v.X;
	v4.y = v.Y;
	v4.z = v.Z;
	v4.w = v.W;
	return v4;
}
FVector2D ToFVector2D(Vector2 v)
{
	return FVector2D(v.x, v.y);
}
Vector2 ToVector2(FVector2D v)
{
	Vector2 v2;
	v2.x = v.X;
	v2.y = v.Y;
	return v2;
}
AActor* ToAActor(const AActorOpaque* actor)
{
	return (AActor*)actor;
}

AActor* ToAActor(AActorOpaque* actor)
{
	return (AActor*)actor;
}

FGuid ToFGuid(Uuid uuid)
{
	return FGuid(uuid.a, uuid.b, uuid.c, uuid.d);
}

Uuid ToUuid(FGuid guid)
{
	Uuid uuid;
	uuid.a = guid.A;
	uuid.b = guid.B;
	uuid.c = guid.C;
	uuid.d = guid.D;
	return uuid;
}
FName ToFName(const UName& name)
{
	auto id = FNameEntryId::FromUnstableInt(name.entry);
	return FName(id, id, name.number);
}
UName ToUName(const FName& name)
{
	UName ret;
	ret.number = name.GetNumber();
	ret.entry = name.GetComparisonIndex().ToUnstableInt();
	return ret;
}