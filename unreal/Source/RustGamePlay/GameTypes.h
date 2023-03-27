#pragma once

/**
 * game camera blend event
 */
UENUM(BlueprintType)
enum class EGameCameraEvent : uint8 {
    None = 0 UMETA(Hidden),
    /**
     * @brief select target
     */
    SelectCharacter,
    SetViewTarget,
    BlendTarget,
    /**
     * @brief mouse wheel event
     */
    ScrollWheel,
    /**
     * @brief spawn a monster
     */
    OnMonsterSpawn,
};

