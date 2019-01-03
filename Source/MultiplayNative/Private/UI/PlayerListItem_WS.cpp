// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerListItem_WS.h"
#include "TextBlock.h"
#include "PlayerList_WS.h"
#include "BattlePS.h"


void UPlayerListItem_WS::UpdateItemList()
{
	if (!IsValid(BattlePS))
		return;

	PlayerName->SetText(FText::FromString(BattlePS->GetPlayerName()));
	KillCount->SetText(FText::FromString(FString::FromInt(BattlePS->KillCount)));
	DeathCount->SetText(FText::FromString(FString::FromInt(BattlePS->DeathCount)));
}