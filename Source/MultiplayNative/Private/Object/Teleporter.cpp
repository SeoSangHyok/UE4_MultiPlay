// Fill out your copyright notice in the Description page of Project Settings.

#include "Teleporter.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "MultiplayNativeCharacter.h"

// Sets default values
ATeleporter::ATeleporter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ATeleporter::OnBoxOverlap);
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
	StaticMeshComp->SetRelativeScale3D(FVector(1.0f,1.0f,0.2f));
	StaticMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -85.0f));
}

// Called when the game starts or when spawned
void ATeleporter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATeleporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATeleporter::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(Cast<AMultiplayNativeCharacter>(OtherActor)))
	{
		// 실게임에서 콘솔명령어를 직접 넣는건 아주 않좋은 방법이니 이런방식은 테스트에서나 활용할것
//		UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), FString(TEXT("open Battle")));


		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Battle")));
	}
}