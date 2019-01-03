// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Ball.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UArrowComponent;
class UProjectileMovementComponent;

UCLASS()
class MULTIPLAYNATIVE_API ABall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION()
	void OnBeginOverapProc(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ShowExplosion();
	void Multi_ShowExplosion_Implementation();
	
public:
	UPROPERTY()
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY()
	USphereComponent* SphereComp;

	UPROPERTY()
	UProjectileMovementComponent* ProjectileMovComp;

	UPROPERTY()
	FTimerHandle ExplosionTimer;
};
