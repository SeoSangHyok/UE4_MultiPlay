// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultiplayNativeCharacter.generated.h"

class UArrowComponent;
class UWidgetComponent;
class UWidgetComponent;

UCLASS(config=Game)
class AMultiplayNativeCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere)
	UArrowComponent* SpawnLocComp;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* WidgetComp;
public:
	AMultiplayNativeCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(ReplicatedUsing = Rep_HP)
	float HP = 50.0f;

protected:
	virtual void BeginPlay() override;

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void OnKeyInput_F();
	void OnKeyFire();
	void OnKeyInput_Tab();
	void OnKeyInput_TabRelease();

	UFUNCTION(Server, Reliable, WithValidation)
	void Serv_SpawnBall();
	bool Serv_SpawnBall_Validate() { return true; }
	void Serv_SpawnBall_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Serv_ReqFire(FVector StartLoc, FVector EndLoc);
	bool Serv_ReqFire_Validate(FVector StartLoc, FVector EndLoc) { return true; }
	void Serv_ReqFire_Implementation(FVector StartLoc, FVector EndLoc);

	UFUNCTION(NetMulticast, unreliable)
	void Multi_ShowEffect(const FString& EffectRef, FVector Loc);
	void Multi_ShowEffect_Implementation(const FString& EffectRef, FVector Loc);

	UFUNCTION()
	void Rep_HP();

public:
	void GetDamage(float InHP);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

