#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/PawnMovementComponent.h>
#include "RustAnimInstance.generated.h"
#define AnimParamCategory "FAnimParams"

USTRUCT(BlueprintType)
struct FAnimHitState {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsHit;
	/**
	* hit world location
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector Location;
	/**
	* hit direction
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector HitDirection;
	/**
	* hit actor
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		AActor* SourceActor;

};
UENUM(BlueprintType)
enum class EAnimParamType : uint8
{
	Bool,
	Float,
	Integer,
	Vector,
};
/*
* hard code animation state
*/
enum class EFixeAnimState: uint8 {
	None = 0,
	InCombat = 1,
	IsDead,
	/**
	 * @brief knockdown
	 */
	KnockDown,
	/**
	 * @brief knock -> standup
	 */
	StandUp,
};
UENUM(BlueprintType)
enum class EGameAnimStateType: uint8 {
	None = 0 UMETA(Hidden),
	InCombat = 1,
	IsDead,
	ShouldMove,
	IsFalling,
	/**
	 * @brief standing up
	 */
	StandUp,
	/**
	 * @brief knocking down, include floating
	 */
	KnockDown,
	IsHit,
	AnimRate,
	WeaponState,
};
USTRUCT(BlueprintType)
struct FAnimParams {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (HiddenByDefault))
		FVector Value;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EAnimParamType ParamType;
	FAnimParams();
	FAnimParams(EAnimParamType Pt);
	void SetBool(bool Bool)
	{
		int32 V = Bool;
		SetInt(V);
	}
	bool GetBool() const
	{
		return GetInt() != 0;
	}
	float GetFloat() const
	{
		return Value.X;
	}
	void SetFloat(float Float)
	{
		Value.X = Float;
	}
	void SetInt(int32 Int)
	{
		Value.X = (double)(*&Int);
	}
	int32 GetInt() const
	{
		return (int)(*&Value.X);
	}
	void SetVector(FVector Vector) { Value = Vector; }
	FVector GetVector() const{ return Value; }

};
UCLASS()
class URustAnimInstance: public UAnimInstance
{
	GENERATED_BODY()

protected:
	// the below functions are the native overrides for each phase
	// Native initialization override point
	virtual void NativeInitializeAnimation() override;
	// Native update override point. It is usually a good idea to simply gather data in this step and 
	// for the bulk of the work to be done in NativeThreadSafeUpdateAnimation.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	// Native thread safe update override point. Executed on a worker thread just prior to graph update 
	// for linked anim instances, only called when the hosting node(s) are relevant
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
public:
	/*
	* set fixed animation state
	*/
	void SetFixedAnimState(EFixeAnimState State, FAnimParams Param);
	/*
	* set dynamic animation state
	*/
	void SetDynAnimState(FName StateName, FAnimParams Param);
	void SetWeapon(int8 Weapon) {
		auto W = WeaponType;
		WeaponType = Weapon;
		if (W != Weapon)
			OnAnimStateChanged(EGameAnimStateType::WeaponState, Weapon);
	}
	int8 GetWeapon()const { return WeaponType; }
	/**
	* set hit state
	*/
	void SetHitState(FAnimHitState State) { 
		HitState = State; 
		if (HitState.IsHit)
			OnAnimStateChanged(EGameAnimStateType::IsHit, 1);
	};
public:
	/**
	 * add param value
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		void AddBool(FName Name, bool Value);
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		void AddFloat(FName Name, float Value);
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		void AddInteger(FName Name, int32 Value);
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		void AddVector(FName Name, FVector Value);
		FORCEINLINE void AddValue(FName Name, FAnimParams Value);
	/**
	 * set param value, fail if value type not correct
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		void SetBool(FName Name, bool Value);
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		void SetFloat(FName Name, float Value);
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		void SetInteger(FName Name, int32 Value);
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		void SetVector(FName Name, FVector Value);
	FORCEINLINE void SetValue(FName Name, FAnimParams Value);

	/**
	 * get param value, fail if value type not correct
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		const FAnimParams& GetValue(FName Name);
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay", meta = (BlueprintThreadSafe))
		bool GetBoolValue(FName Name) const;
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		float GetFloatValue(FName Name) const;
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		int32 GetIntValue(FName Name) const;
	/*
	* get and consume bool trigger value
	*/
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		bool ConsumeBoolValue(FName Name);
	/*
	* set animation play rate, duration < 0 mean forever
	*/
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		void SetAnimPlayRate(float Rate, float Duration = -1.0);
	/*
	* get animation play rate
	*/
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		float GetAnimPlayRate() const { return AnimPlayRate; }
	/*
	* get animation play rate tick (Rate != 1.0)
	*/
	UFUNCTION(BlueprintCallable, Category = "Rust|GamePlay")
		float GetAnimRateDuration() const { return AnimPlayRateTick; }
	/**
	* on animation state changed
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Rust")
		void OnAnimStateChanged(EGameAnimStateType State, int32 Value);
	/**
	 * @brief set freeze state
	 * @param Duration freeze time
	 * @param OverrideDuration override freezing time duration or additional duration
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust| GamePlay")
		void SetFreeze(float Duration, bool OverrideDuration = true);
	/**
	 * @brief cancle freeze state
	 */
	UFUNCTION(BlueprintCallable, Category = "Rust| GamePlay")
		void SetNotFreeze();

private:
	/**
	* update montage play rate
	*/
	void UpdateMontageRate(float Rate);

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation Properties", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FAnimParams> Properties;
	/*
	* if game is in combat state
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
		uint8 InCombat: 1;
	uint8 bFreezing : 1;
	/*
	* if unit is dead
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
	uint8 IsDead: 1;
	/*
	* unit movespeed
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
	uint8 ShouldMove: 1;

	/*
	* unit is falling
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
		uint8 IsFalling: 1;
	/*
	* unit weapon type
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
		int32 WeaponType;
	/*
	* unit movespeed
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
		float MoveSpeed;
	/*
	* unit Velocity
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
		FVector Velocity;
	/*
	* unit movespeed threshold
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
		float MoveSpeedThreshold = 3.0;
	/*
	* unit accsleration
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
		float Acceleration;

	/*
	* unit animation play rate
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
		float AnimPlayRate = 1.0;

	/*
	* unit animation play rate change duration
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
		float AnimPlayRateDuration = -1.0;
	float AnimPlayRateTick = 0.0;
	/**
	* anim freeze duration
	*/
	float FreezingDuration;
	/*
	* unit move dir(when turning left or right)
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
		FVector2f MoveDir;
	/*
	* unit is Hit
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
		FAnimHitState HitState;
	UPROPERTY()
		UPawnMovementComponent* BaseMovement;
	UPROPERTY()
		UCharacterMovementComponent* CharacterMovement;
};

