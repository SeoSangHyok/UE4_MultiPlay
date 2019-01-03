// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleResult_WS.h"
#include "VerticalBox.h"
#include "PlayerResultItem_WS.h"



void UBattleResult_WS::MakeBattleResult(const TArray<FPlayerResultItem>& PlayerResults)
{
	FSoftObjectPath WidgetClassLoader(TEXT("WidgetBlueprint'/Game/UI/PlayerResultItem_WB.PlayerResultItem_WB_C'"));
	UClass* WidgetClass = Cast<UClass>(WidgetClassLoader.TryLoad());

	ResultListBox->ClearChildren();

	UPlayerResultItem_WS* WidgetColume = CreateWidget<UPlayerResultItem_WS>(GetOwningPlayer(), WidgetClass);
	if (WidgetColume)
	{
		ResultListBox->AddChildToVerticalBox(WidgetColume);
	}

	for (const FPlayerResultItem& PlayerResultItem : PlayerResults)
	{
		UPlayerResultItem_WS* PlayerResultItemWidget = CreateWidget<UPlayerResultItem_WS>(GetOwningPlayer(), WidgetClass);
		if (PlayerResultItemWidget)
		{
			PlayerResultItemWidget->SetPlayerResult(&PlayerResultItem);
			ResultListBox->AddChildToVerticalBox(PlayerResultItemWidget);
		}
	}
}