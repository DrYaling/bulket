#pragma once
#include "CoreTypes.h"

//unit type
UENUM(BlueprintType)
enum class ERustUnitType : uint8
{
    /**
     * @brief not initialized unit
     */
    Undefined = 0,
    /**
     * @brief static world object
     */
    WorldObject,
    /**
     * @brief creature with ai controller
     */
    Creature,
    /**
     * @brief character controlled by player
     */
    Character,
    /**
     * @brief pet controlled by ai & player
     */
    Pet,
};
UENUM(BlueprintType)
enum class EGameNotifyEventType : uint8 {
    None = 0 UMETA(Hidden),
    /**
    * unit attr of type(SubEvent) changed
    */
    UnitAttributeChanged,
    /**
    * skill of skill bar(subEvent) casted
    */
    SkillCasted,
    /**
    * skill of skill bar (SubEvent) change to EventValue
    */
    SkillChanged,
    /**
    * target changed to EventValue
    */
    OnTargetChanged,
    /**
    * game input, used for gcd control
    */
    OnGameInput,
};

//thread unsafe
struct NativeString {
    char* utfStr;
    uint32 size;
};
//thread unsafe
struct RefString {
    char* utfStr;
    void* str_ref;
    uint32 size;
};
enum RustApiLogLevel
{
    Error = 1,
    Warn = 2,
    Info = 3,
    Trace = 4,
};
UENUM(BlueprintType)
enum class EGameInputType : uint8 {
    None = 0 UMETA(Hidden),
    ControlA,
    ControlB,
    ControlC,
    ControlD,
    SkillBar1,
    SkillBar2,
    SkillBar3,
    SkillBar4,
    SkillBar5,
};
enum class REndPlayReason : uint8_t
{
    /** When the Actor or Component is explicitly destroyed. */
    Destroyed,
    /** When the world is being unloaded for a level transition. */
    LevelTransition,
    /** When the world is being unloaded because PIE is ending. */
    EndPlayInEditor,
    /** When the level it is a member of is streamed out. */
    RemovedFromWorld,
    /** When the application is being exited. */
    Quit,
};

enum class ActionState : uint8_t {
    Pressed = 0,
    Released = 1,
    Held = 2,
};

enum class ActorClass : uint32_t {
    RustActor = 0,
    CameraActor = 1,
    RustCharacter = 2,
    RustPawn = 3,
    EngineActor = 4,
};

enum class ActorComponentType : uint32_t {
    Primitive,
};

enum class CollisionShapeType : uint32_t {
    Box,
    Capsule,
    Sphere,
};

enum class EventType : uint32_t {
    ActorSpawned = 0,
    ActorBeginOverlap = 1,
    ActorEndOverlap = 2,
    ActorOnHit = 3,
    ActorDestroy = 4,
};


enum class ResultCode : uint8_t {
    Success = 0,
    Panic = 1,
};

enum class UObjectType : uint32_t {
    UClass,
};

struct Vector3 {
    float x;
    float y;
    float z;
};
struct Vector2 {
    float x;
    float y;
};
struct Quaternion {
    float x;
    float y;
    float z;
    float w;
};

typedef Quaternion Vector4;
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct Utf8Str {
    const char* ptr;
    uintptr_t len;
};

struct Uuid {
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
};

struct IntPoint
{
    int32 x, y;
};
struct Transform
{
    Quaternion Rotation;
    Vector3 Location;
    Vector3 Scale;
};
struct UName
{
    uint32 entry;
    uint32 number;
};
using UObjectOpague = void;
using UClassOpague = void;
using AActorOpaque = void;
UENUM(BlueprintType)
enum class EAttributeType : uint8 {
    None = 0,
    //精气-感知-术法
    Senseless,
    //血气-外循环
    BloodAura,
    //神-内外循环-罡气和回复
    Circulating,
    Health,
    MaxHealth,
    Mana,
    MaxMana,
    Critical,
    CriticalDamage,
    Hast,
    Speed,
    MaxCount
};
enum class EGameComponentType : uint8 {
    CharacterMovement,
    SkeletalMesh,
    Camera,
    Capsule,
    Rust,
};
struct ComponentInfo {
    EGameComponentType ComponentType;
    void* Component;
};