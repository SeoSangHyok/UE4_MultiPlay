// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerResultItem_WS.h"
#include "TextBlock.h"
#include "Kismet/KismetTextLibrary.h"
#include "StructureDefines.h"



void  UPlayerResultItem_WS::SetPlayerResult(const FPlayerResultItem* PlayerResultItem)
{
	if (nullptr == PlayerResultItem)
		return;

	if (IsValid(Rank))
	{
		Rank->SetText(UKismetTextLibrary::Conv_IntToText(PlayerResultItem->Rank));
	}

	if (IsValid(PlayerName))
	{
		PlayerName->SetText(FText::FromString(PlayerResultItem->PlayerName));
	}

	if (IsValid(KillCount))
	{
		KillCount->SetText(UKismetTextLibrary::Conv_IntToText(PlayerResultItem->Kill));
	}

	if (IsValid(DeathCount))
	{
		DeathCount->SetText(UKismetTextLibrary::Conv_IntToText(PlayerResultItem->Death));
	}

	if (IsValid(Score))
	{
		Score->SetText(UKismetTextLibrary::Conv_IntToText(PlayerResultItem->Score));
	}
}