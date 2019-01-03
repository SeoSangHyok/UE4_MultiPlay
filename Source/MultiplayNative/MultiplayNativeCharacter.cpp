// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "MultiplayNativeCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ArrowComponent.h"
#include "Ball.h"
#include "Engine/World.h"
#include "WidgetComponent.h"
#include "HP_WS.h"
#include "UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Engine/CollisionProfile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "BattlePC.h"
#include "BattlePS.h"
#include "TimerManager.h"
#include "BattleGM.h"
#include "EnumDefines.h"

//////////////////////////////////////////////////////////////////////////
// AMultiplayNativeCharacter

AMultiplayNativeCharacter::AMultiplayNativeCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	USkeletalMeshComponent* SkeletalMeshComp = GetMesh();
	SkeletalMeshComp->SetCollisionProfileName(FName(TEXT("CharacterMeshForFileLineTrace")));
	
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SetRelativeLocation(FVector(0.0f,0.0f,115.0f));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	SpawnLocComp = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnLocComp"));
	SpawnLocComp->SetupAttachment(RootComponent);
	SpawnLocComp->SetRelativeLocation(FVector(78.0f, 0.0f, 60.0f));

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComp->SetupAttachment(RootComponent);
	WidgetComp->SetDrawAtDesiredSize(true);
	WidgetComp->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	FSoftObjectPath WidgetLoader(TEXT("WidgetBlueprint'/Game/UI/HP_WB.HP_WB_C'"));
	UClass* WidgetClass = Cast<UClass>(WidgetLoader.TryLoad());
	if (IsValid(WidgetClass))
	{
		WidgetComp->SetPivot(FVector2D(0.5f, 0.5f));
		WidgetComp->SetWidgetClass(WidgetClass);
	}
}

void AMultiplayNativeCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayNativeCharacter, HP);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiplayNativeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMultiplayNativeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMultiplayNativeCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMultiplayNativeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMultiplayNativeCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMultiplayNativeCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMultiplayNativeCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMultiplayNativeCharacter::OnResetVR);

	PlayerInputComponent->BindKey(EKeys::F, EInputEvent::IE_Pressed, this, &AMultiplayNativeCharacter::OnKeyInput_F);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, EInputEvent::IE_Pressed, this, &AMultiplayNativeCharacter::OnKeyFire);

	PlayerInputComponent->BindKey(EKeys::Tab, EInputEvent::IE_Pressed, this, &AMultiplayNativeCharacter::OnKeyInput_Tab);
	PlayerInputComponent->BindKey(EKeys::Tab, EInputEvent::IE_Released, this, &AMultiplayNativeCharacter::OnKeyInput_TabRelease);
}

void AMultiplayNativeCharacter::BeginPlay()
{
	Super::BeginPlay();
	Rep_HP();
}

void AMultiplayNativeCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMultiplayNativeCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMultiplayNativeCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMultiplayNativeCharacter::OnKeyInput_F()
{
	Serv_SpawnBall();
}

void AMultiplayNativeCharacter::OnKeyFire()
{
	if (HP <= 0.0f)
		return;

	FVector StartLoc = FollowCamera->GetComponentLocation();
	FVector EndLoc = StartLoc + (FollowCamera->GetForwardVector() * 10000.0f);

	Serv_ReqFire(StartLoc, EndLoc);
}

void AMultiplayNativeCharacter::OnKeyInput_Tab()
{
	ABattlePC* BattlePC = Cast<ABattlePC>(GetController());
	if (IsValid(BattlePC))
	{
		BattlePC->ShowPlayerList();
	}
}

void AMultiplayNativeCharacter::OnKeyInput_TabRelease()
{
	ABattlePC* BattlePC = Cast<ABattlePC>(GetController());
	if (IsValid(BattlePC))
	{
		BattlePC->HIdePlayerList();
	}
}

void AMultiplayNativeCharacter::Serv_SpawnBall_Implementation()
{
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocComp->GetComponentLocation());
		SpawnTransform.SetRotation(GetControlRotation().Quaternion());

		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Instigator = this;

		ABall* Ball = World->SpawnActor<ABall>(ABall::StaticClass(), SpawnTransform, ActorSpawnParameters);
		Ball->SetOwner(this);
	}
}

void AMultiplayNativeCharacter::Serv_ReqFire_Implementation(FVector StartLoc, FVector EndLoc)
{
	if (!HasAuthority() || HP <= 0.0f)
		return;

	const UCollisionProfile* CollisionProfile = UCollisionProfile::Get();
	if (!IsValid(CollisionProfile))
		return;


	int CollisionIndex = INDEX_NONE;
	for (int i = 0; i<ECollisionChannel::ECC_MAX; i++)
	{
		FName CheckCollisionName = CollisionProfile->ReturnChannelNameFromContainerIndex(i);
		if (CheckCollisionName == TEXT("FireLineTrace"))
		{
			CollisionIndex = i;
			break;
		}
	}

	if (INDEX_NONE == CollisionIndex)
		return;

	TEnumAsByte<ECollisionChannel> CollisionChannelAsByte(CollisionIndex);
	ETraceTypeQuery TraceTypeQuery = CollisionProfile->ConvertToTraceType(CollisionChannelAsByte.GetValue());
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	FHitResult HitResult;

	EDrawDebugTrace::Type TraceType = EDrawDebugTrace::Type::None;
	bool TraceResult = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLoc, EndLoc, TraceTypeQuery, false, ActorsToIgnore, TraceType, HitResult, true);
	UKismetSystemLibrary::PrintString(GetWorld(), FText::FromName(HitResult.BoneName).ToString());

	if (false == TraceResult)
		return;

 	ABattleGM* BattleGM = Cast<ABattleGM>(UGameplayStatics::GetGameMode(GetWorld()));

	if (IsValid(BattleGM) && BattleGM->GetBattleState() == EBattleState::Playing)
	{
		UGameplayStatics::ApplyPointDamage(HitResult.Actor.Get(), 10.0f, (HitResult.Normal * -1.0f), HitResult, GetController(), nullptr, nullptr);
	}

	Multi_ShowEffect(TEXT("ParticleSystem'/Game/VFX/Particles/P_Explosion.P_Explosion'"), HitResult.ImpactPoint);
}

void AMultiplayNativeCharacter::Multi_ShowEffect_Implementation(const FString& EffectRef, FVector Loc)
{
	FSoftObjectPath Loader(EffectRef);
	UParticleSystem* ParticleSystem = Cast<UParticleSystem>(Loader.TryLoad());
	if (IsValid(ParticleSystem))
	{
		UParticleSystemComponent* ParticleSystemComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, Loc, FRotator::ZeroRotator);
		if (IsValid(ParticleSystemComp))
		{
			ParticleSystemComp->SetRelativeScale3D(FVector(0.2f));
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("IsValid(ParticleSystem) is not Valid!!!"));
	}
}

void AMultiplayNativeCharacter::Rep_HP()
{
	if (IsValid(WidgetComp))
	{
		UHP_WS* HP_WS = Cast<UHP_WS>(WidgetComp->GetUserWidgetObject());
		if (IsValid(HP_WS))
		{
			HP_WS->SetHP(HP);
		}
	}

	if (HP <= 0.0f)
	{
		FSoftObjectPath DeadMontageList[]{
			TEXT("AnimMontage'/Game/AnimStarterPack/Death_1_Montage.Death_1_Montage'"),
			TEXT("AnimMontage'/Game/AnimStarterPack/Death_2_Montage.Death_2_Montage'"),
			TEXT("AnimMontage'/Game/AnimStarterPack/Death_3_Montage.Death_3_Montage'"),
		};

		int DeadMotionIndex = FMath::Rand() % 3;
		PlayAnimMontage(Cast<UAnimMontage>(DeadMontageList[DeadMotionIndex].TryLoad()));
		GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("DeadBody")));
		GetCharacterMovement()->SetMovementMode(MOVE_None);

		if (IsValid(WidgetComp))
		{
			WidgetComp->SetVisibility(false);
		}
	}
}

void AMultiplayNativeCharacter::GetDamage(float InHP)
{
}

float AMultiplayNativeCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HP <= 0)
		return 0.0f;

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		if (nullptr != PointDamageEvent)
		{
			if (PointDamageEvent->HitInfo.BoneName == TEXT("head"))
			{
				HP -= 50.0f;
			}
			else
			{
				HP -= ActualDamage;
			}

			if (HP <= 0.0f)
			{
				HP = 0.0f;
				ABattlePC* BattlePC = Cast<ABattlePC>(GetController());
				if (IsValid(BattlePC))
				{
					BattlePC->OnPawnIsDead();
				}

				ABattlePC* EventInstigatorPC = Cast<ABattlePC>(EventInstigator);
				if (IsValid(EventInstigatorPC))
				{
					ABattlePS* EventInstigatorPS = Cast<ABattlePS>(EventInstigatorPC->PlayerState);
					if (IsValid(EventInstigatorPS))
					{
						EventInstigatorPS->KillCount++;
					}
				}
			}
		}
	}
	else if ( DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
		if (nullptr != RadialDamageEvent)
		{
			HP -= ActualDamage;
		}

		if (HP <= 0.0f)
		{
			HP = 0.0f;
			ABattlePC* BattlePC = Cast<ABattlePC>(GetController());
			if (IsValid(BattlePC))
			{
				BattlePC->OnPawnIsDead();
			}

			ABattlePC* EventInstigatorPC = Cast<ABattlePC>(EventInstigator);
			if (IsValid(EventInstigatorPC) && BattlePC != EventInstigatorPC)
			{
				ABattlePS* EventInstigatorPS = Cast<ABattlePS>(EventInstigatorPC->PlayerState);
				if (IsValid(EventInstigatorPS))
				{
					EventInstigatorPS->KillCount++;
				}
			}
		}
	}
	else
	{
		HP -= ActualDamage;

		if (HP <= 0.0f)
		{
			HP = 0.0f;
			ABattlePC* BattlePC = Cast<ABattlePC>(GetController());
			if (IsValid(BattlePC))
			{
				BattlePC->OnPawnIsDead();
			}

			ABattlePC* EventInstigatorPC = Cast<ABattlePC>(EventInstigator);
			if (IsValid(EventInstigatorPC) && BattlePC != EventInstigatorPC)
			{
				ABattlePS* EventInstigatorPS = Cast<ABattlePS>(EventInstigatorPC->PlayerState);
				if (IsValid(EventInstigatorPS))
				{
					EventInstigatorPS->KillCount++;
				}
			}
		}
	}

	Rep_HP();

	if (HP <= 0.0f)
	{
		FTimerDelegate TimerCallback;
		TimerCallback.BindLambda([&]
		{
			Destroy();
		});

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, TimerCallback, 5.0f, false);
	}

	return ActualDamage;
}

void AMultiplayNativeCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMultiplayNativeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMultiplayNativeCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMultiplayNativeCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
