#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RustAnimInstance.generated.h"
#define AnimParamCategory "FAnimParams"
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


private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation Properties", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FAnimParams> Properties;
	/*
	* if game is in combat state
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GamePlay States", meta = (AllowPrivateAccess = "true"))
	bool InCombat;

};

