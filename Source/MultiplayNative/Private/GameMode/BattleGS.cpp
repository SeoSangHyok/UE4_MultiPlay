// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleGS.h"
#include "UnrealNetwork.h"
#include "BattlePC.h"
#include "Kismet/GameplayStatics.h"
#include "Battle_WS.h"
#include "BattlePS.h"



void ABattleGS::BeginPlay()
{
	Super::BeginPlay();
}

void ABattleGS::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleGS, MainMessage);
	DOREPLIFETIME(ABattleGS, RoomNo);
	DOREPLIFETIME(ABattleGS, BattleState);
}

void ABattleGS::Rep_MainMessage()
{
	ABattlePC* BattlePC = Cast<ABattlePC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(BattlePC))
	{
		UBattle_WS* Battle_WS = BattlePC->GetBattleWidget();
		if (IsValid(Battle_WS))
		{
			Battle_WS->SetMainMessage(MainMessage);
		}
	}
}

ABattlePS* ABattleGS::GetPlayerState(const FString& PlayerName)
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState->GetPlayerName() == PlayerName)
		{
			ABattlePS* BattlePS = Cast<ABattlePS>(PlayerState);
			return BattlePS;
		}
	}

	return nullptr;
}