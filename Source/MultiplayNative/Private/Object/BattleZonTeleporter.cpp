// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleZonTeleporter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "MultiplayNativeCharacter.h"
#include "LobbyPC.h"
#include "WidgetBlueprintLibrary.h"


// Sets default values
ABattleZonTeleporter::ABattleZonTeleporter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ABattleZonTeleporter::OnBoxOverlap);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &ABattleZonTeleporter::OnBoxOverlapEnd);
	SetRootComponent(BoxComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> CubeMaterial(TEXT("Material'/Engine/EditorMeshes/ColorCalibrator/M_ChromeBall.M_ChromeBall'"));

	// 아래 방식으로 로딩하는 법도 있으니 참고
	// 	FSoftObjectPath SoftObjCube(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	// 	StaticMeshComp->SetStaticMesh(Cast<UStaticMesh>(SoftObjCube.TryLoad()));

	StaticMeshComp->SetStaticMesh(CubeMesh.Object);
	StaticMeshComp->SetMaterial(0, CubeMaterial.Object);
	float FloorHeight = 10.0f;
	StaticMeshComp->SetRelativeScale3D(FVector(1.0f, 1.0f, (FloorHeight * 0.01f))); // CubeMesh 기본 사이즈는 정육면체 50.0f
	StaticMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f + (FloorHeight * 0.5f)));
}

// Called when the game starts or when spawned
void ABattleZonTeleporter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABattleZonTeleporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABattleZonTeleporter::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMultiplayNativeCharacter* MultiplayNativeCharacter = Cast<AMultiplayNativeCharacter>(OtherActor);

	if (IsValid(MultiplayNativeCharacter))
	{
		ALobbyPC* LobbyPC = Cast<ALobbyPC>(MultiplayNativeCharacter->GetController());
		if (IsValid(LobbyPC) && LobbyPC->IsLocalController())
		{
			LobbyPC->SetRoomlistVisible(true);

			LobbyPC->bShowMouseCursor = true;
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(LobbyPC);
		}
	}
}

void ABattleZonTeleporter::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMultiplayNativeCharacter* MultiplayNativeCharacter = Cast<AMultiplayNativeCharacter>(OtherActor);

	if (IsValid(MultiplayNativeCharacter))
	{
		ALobbyPC* LobbyPC = Cast<ALobbyPC>(MultiplayNativeCharacter->GetController());
		if (IsValid(LobbyPC) && LobbyPC->IsLocalController())
		{
			LobbyPC->SetRoomlistVisible(false);

			LobbyPC->bShowMouseCursor = false;
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(LobbyPC);
		}
	}
}