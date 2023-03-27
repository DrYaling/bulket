// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMontage.h"
#include "UObject/Object.h"
#include "SkillAsset.generated.h"
class IRustObjectInterface;
USTRUCT(BlueprintType)
struct FSkillConfig {
    GENERATED_BODY()

    /**
     * @brief skill can be terminated
     */
    UPROPERTY(BlueprintReadWrite, DisplayName = "Skill can be terminated at ComboState", EditAnywhere, Category = "Rust")
	bool CanTerminate = true;
    /**
     * @brief combo skill count
     */
    UPROPERTY(BlueprintReadWrite, DisplayName = "Skill Combo List Count", EditAnywhere, Category = "Rust")
    uint8 ComboCount = 0;
    UPROPERTY(BlueprintReadOnly, DisplayName = "Skill Id", EditAnywhere, Category = "Rust")
    int32 Id = 0;
    UPROPERTY(BlueprintReadWrite, DisplayName = "Skill CoolDown", EditAnywhere, Category = "Rust")
    float Cool = 0.0f;
    UPROPERTY(BlueprintReadWrite, DisplayName = "Skill SkillPreAnim state time", EditAnywhere, Category = "Rust")
    float PreState = 0.0f;
    UPROPERTY(BlueprintReadWrite, DisplayName = "Skill PreTerminate state time", EditAnywhere, Category = "Rust")
    float PreTerminate = 0.0f;
    UPROPERTY(BlueprintReadWrite, DisplayName = "Skill SkillCasting state time", EditAnywhere, Category = "Rust")
    float Casting = 0.0f;
    UPROPERTY(BlueprintReadWrite, DisplayName = "Skill SkillTail time", EditAnywhere, Category = "Rust")
    float Tail = 0.0f;
    UPROPERTY(BlueprintReadWrite, DisplayName = "Skill Combo state duration", EditAnywhere, Category = "Rust")
    float ComboDura = 0.0f;
    /**
     * @brief next combo skill id shared same input key
     */
    UPROPERTY(BlueprintReadWrite, DisplayName = "NextDefaultCombo", EditAnywhere, Category = "Rust")
    int32 ComboDefaultSkill = 0;
    float DamageInterval = 0.0f;
    int32 ComboSkills[4];
    float DamageRanges[4] = {0.0};
};
UENUM(BlueprintType)
enum class ESkillNormalCastState: uint8 {
    None UMETA(Hidden),
    /**
     * skill is ready
     */
    Ready,
    /**
     * skill is not ready
     */
    Cooling,
    /**
     * skill is not valid
     */
    Disabled,
    /**
     * @brief skill casting state (pre animation)
     */
    SkillPreAnim,
    /**
     * @brief skill casting state (can be terminated by dodge)
     */
    PreTerminate,
    /**
     * @brief skill is casting
     */
    SkillCasting,
    /**
     * @brief skill is in combo state (can be interrupted by next combo skill)
     */
    SkillCombo,
    /**
     * @brief skill late animation, can not be interrupted by self
     */
    SkillTail,
    /**
     * @brief skill cast finished, next frame will either enter Ready or enter Cooling
     */
    SkillFinish
};
//skill combo
USTRUCT(BlueprintType)
struct FSkillComboState {
    GENERATED_BODY()

        UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rust")
        //可否打断
        bool CanTerminate;
        //combo 技能数
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rust")
    uint8 ComboCount;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rust")
        //持续帧
	float Duration;
        //连击帧和连击招数列表,最大支持4种连招
//    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rust")
	int32 combo_skills[4];
};

//skill state
USTRUCT(BlueprintType)
struct FSkillCastState {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rust")
        ESkillNormalCastState State;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rust")
    float StateDuration;
};

//DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FSkillTickSignature, USkillAsset, OnEventTick, float, DeltaTime);
//DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FSkillEventSignature, USkillAsset, OnSkillEvent, ESkillNormalCastState, State, float, Duration);
/**
 * skill asset blueprint
 */
UCLASS(config = Game, BlueprintType, Blueprintable)
class PROJECTU_API USkillAsset : public UObject
{
    GENERATED_BODY()

        USkillAsset();
protected:
    virtual void BeginDestroy() override;
public:
    void InitSkillAsset(IRustObjectInterface* Owner, int32 Id);
    void UnloadSkill();
    /**
     * skill state changed
     */
    void OnSkillStateChanged(ESkillNormalCastState, float);
    /**
     * @brief 更新逻辑,计算动画和技能帧
     */
    void ProcessTick(float DeltaTime);
    /**
     * on anim notify
     */
    void OnAnimNotify(struct FAnimNotifyValue NotifyValue);
    static void UnloadHandlers();
public:
    /**
    * get combo skill id by index
    */
    UFUNCTION(BlueprintCallable, Category = "Rust")
    int32 GetComboSkill(int32 index);
    /**
    * get combo state
    */
    UFUNCTION(BlueprintCallable, Category = "Rust")
        FSkillComboState& GetComboState();
    /**
    * play anim montage
    */
    UFUNCTION(BlueprintCallable, Category = "Rust")
        float PlayMontage(float InPlayRate = 1.f, FName StartSectionName = NAME_None);
    /**
     * @brief set skill config from blueprint
     * @param Config config constructed from LoadConfigFromMontage
     * @param ComboSkills combo skills, if contains 0 mean any skill(include self) could follow this skill combo
     */
    UFUNCTION(BlueprintCallable, Category = "Rust")
        void SetSkillConfig(FSkillConfig Config, TArray<int32> ComboSkills);
    /**
     * @brief load config from skill montage
     * @return skill config
     */
    UFUNCTION(BlueprintCallable, Category = "Rust")
        void LoadConfigFromMontage(FName StartSectionName = NAME_None);
    /**
    * skill tick
    */
  //  UPROPERTY(BlueprintAssignable, Category = "Rust")
  //      FSkillTickSignature OnEventTick;
    /**
    * skill event changed
    */
    UFUNCTION(BlueprintImplementableEvent, Category = "Rust")
    void OnSkillEvent(ESkillNormalCastState State, float Duration);
    /**
    * on skill initialize
    */
    UFUNCTION(BlueprintImplementableEvent, Category = "Rust")
    void OnSkillInitialize();
private:
    /*******               skill native apis                 *******/
private:
    /**
     * @brief use custom montage animation blend settings to play skill montage
     */
    UPROPERTY(BlueprintReadWrite, Category = "Rust", meta = (AllowPrivateAccess = "true"))
        bool UseCustomBlendSettings;
    UPROPERTY(BlueprintReadOnly, Category = "Rust", meta = (AllowPrivateAccess = "true"))
    int32 SkillId;
    void* SkillHandler;
    UPROPERTY(BlueprintReadOnly, DisplayName = "CachedComboState", Category = "Rust", meta = (AllowPrivateAccess = "true"))
        FSkillComboState CurrentCombo;
        /**
         * current cast state
         */
    UPROPERTY(BlueprintReadOnly, DisplayName = "SkillState", Category = "Rust", meta = (AllowPrivateAccess = "true"))
        FSkillCastState CurrentCastState;
    UPROPERTY(BlueprintReadOnly, DisplayName = "SkillOwner", Category = "Rust", meta = (AllowPrivateAccess = "true"));
        TScriptInterface<IRustObjectInterface> OwnerUnit;
    UPROPERTY(BlueprintReadWrite, Category = "Rust", meta = (AllowPrivateAccess = "true"))
        TSoftObjectPtr<UAnimMontage> SkillMontage;
    /**
     * @brief montage animation blend settings while another montage is playing
     */
    UPROPERTY(BlueprintReadWrite, Category = "Rust", meta = (AllowPrivateAccess = "true"))
        FMontageBlendSettings BlendSettings;

    /**
     * @brief Default Combo Skills
     */
    UPROPERTY(BlueprintReadWrite, Category = "Rust", meta = (AllowPrivateAccess = "true"))
        TArray<int32> DefaultComboSkills;
};
