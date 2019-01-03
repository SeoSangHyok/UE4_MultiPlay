// Fill out your copyright notice in the Description page of Project Settings.

#include "BattlePS.h"
#include "UnrealNetwork.h"




void ABattlePS::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattlePS, KillCount);
	DOREPLIFETIME(ABattlePS, DeathCount);
}

void ABattlePS::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	ABattlePS* NewBattlePS = Cast<ABattlePS>(PlayerState);
	if (IsValid(NewBattlePS))
	{
		NewBattlePS->KillCount = KillCount;
		NewBattlePS->DeathCount = DeathCount;
	}
}