// Fill out your copyright notice in the Description page of Project Settings.

#include "Ball.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMesh.h"
#include "ConstructorHelpers.h"
#include "Materials/Material.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MultiplayNativeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "TimerManager.h"
#include "BattleGM.h"


// Sets default values
ABall::ABall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(FName(TEXT("SphereComp")));
	SetRootComponent(SphereComp);

	SphereComp->SetCollisionProfileName(TEXT("WorldDynamic"));
	SphereComp->SetSimulatePhysics(true);
	SphereComp->SetSphereRadius(12.5f);
//	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	//SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABall::OnBeginOverapProc);
	SphereComp->SetNotifyRigidBodyCollision(true);

	SphereComp->bGenerateOverlapEvents = false;
	SphereComp->SetCollisionResponseToAllChannels(ECR_Block);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SphereComp->SetLinearDamping(1.0f);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("StaticMeshComp")));
	StaticMeshComp->SetupAttachment(RootComponent);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComp->bGenerateOverlapEvents = false;
	StaticMeshComp->SetRelativeScale3D(FVector(0.25f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> SphereMat(TEXT("Material'/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial'"));

	StaticMeshComp->SetStaticMesh(SphereMesh.Object);
	StaticMeshComp->SetMaterial(0, SphereMat.Object);

	ProjectileMovComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovComp"));
	ProjectileMovComp->InitialSpeed = 1500.0f;
	ProjectileMovComp->Velocity = FVector(1.0f,0.0f,0.7f);

	InitialLifeSpan = 5.0f;


	SetReplicates(true);

	//SetLifeSpan(5.0f);
}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();


//	ExplosionTimer
	if (HasAuthority())
	{
		FTimerDelegate TimerCallback;
		TimerCallback.BindLambda([&]
		{
			if (HasAuthority())
			{
				ABattleGM* BattleGM = Cast<ABattleGM>(UGameplayStatics::GetGameMode(GetWorld()));
				if (IsValid(BattleGM) && BattleGM->GetBattleState() == EBattleState::Playing)
				{
					TArray<AActor*> DamageIgnoreActors;
					UGameplayStatics::ApplyRadialDamage(GetWorld(), 20.0f, GetActorLocation(), 500.0f, UDamageType::StaticClass(), DamageIgnoreActors, this, GetInstigatorController(), true, ECC_WorldStatic);
				}

				Multi_ShowExplosion();
				SetLifeSpan(0.1f);
			}
		});

		GetWorldTimerManager().SetTimer(ExplosionTimer, TimerCallback, 3.0f, false);
	}
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABall::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

// 	if (HasAuthority())
// 	{
// 		TArray<AActor*> DamageIgnoreActors;
// 		UGameplayStatics::ApplyRadialDamage(GetWorld(), 20.0f, HitLocation, 200.0f, UDamageType::StaticClass(), DamageIgnoreActors);
// 		Destroy();
// 		Multi_ShowExplosion_Implementation();
// 	}
// 	else
// 	{
// 		Multi_ShowExplosion_Implementation();
// 	}

}

void ABall::OnBeginOverapProc(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMultiplayNativeCharacter* MultiplayNativeCharacter = Cast<AMultiplayNativeCharacter>(OtherActor);
	if (IsValid(MultiplayNativeCharacter))
	{
		if (IsValid(GetOwner()) && GetOwner() != MultiplayNativeCharacter)
		{
			if (HasAuthority())
			{
				UGameplayStatics::ApplyDamage(MultiplayNativeCharacter, 10.0f, nullptr, this, UDamageType::StaticClass());
				Destroy();
				Multi_ShowExplosion();
			}
			else
			{
				Multi_ShowExplosion();;
			}
		}
	}
}

void ABall::Multi_ShowExplosion_Implementation()
//void ABall::Multi_ShowExplosion()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ABall::Multi_ShowExplosion_Implementation()"));

	FSoftObjectPath Loader(TEXT("ParticleSystem'/Game/VFX/Particles/P_Explosion.P_Explosion'"));
	UParticleSystem* ParticleSystem = Cast<UParticleSystem>(Loader.TryLoad());
	if (!IsValid(ParticleSystem))
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("IsValid(ParticleSystem) is not Valid!!!"));
	}

	FTransform EmitterTransform = GetActorTransform();
	EmitterTransform.SetScale3D(FVector(3.0f));

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, EmitterTransform);
}