// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerList_WS.h"
#include "VerticalBox.h"
#include "PlayerListItem_WS.h"
#include "BattlePS.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"

bool UPlayerList_WS::Initialize()
{
	Super::Initialize();

// 	if (IsValid(GetOwningPlayer()))
// 	{
// 		AGameStateBase* GameStateBase = UGameplayStatics::GetGameState(GetOwningPlayer()->GetWorld());
// 		if (IsValid(GameStateBase))
// 		{
// 			for (APlayerState* PlayerState : GameStateBase->PlayerArray)
// 			{
// 				ABattlePS* BattlePS = Cast<ABattlePS>(PlayerState);
// 				if (IsValid(BattlePS))
// 				{
// 					AddPlayer(BattlePS);
// 				}
// 			}
// 		}
// 	}
// 
// 	UpdateItemList();

	return true;
}

void UPlayerList_WS::AddPlayer(ABattlePS* InBattlePS)
{
	for (int i = 0; i < PlayerListBox->GetChildrenCount(); i++)
	{
		UPlayerListItem_WS* PlayerListItem = Cast<UPlayerListItem_WS>(PlayerListBox->GetChildAt(i));
		if (PlayerListItem->BattlePS == InBattlePS)
			return;
	}
	
	FSoftObjectPath WidgetClassLoader(TEXT("WidgetBlueprint'/Game/UI/PlayerListItem_WB.PlayerListItem_WB_C'"));
	UClass* WidgetClass = Cast<UClass>(WidgetClassLoader.TryLoad());
	if (IsValid(WidgetClass))
	{
		UPlayerListItem_WS* NewPlayerListItem = CreateWidget<UPlayerListItem_WS>(GetOwningPlayer(), WidgetClass);
		if (IsValid(NewPlayerListItem))
		{
			NewPlayerListItem->BattlePS = InBattlePS;
			PlayerListBox->AddChild(NewPlayerListItem);
			NewPlayerListItem->UpdateItemList();
		}
	}
}

void UPlayerList_WS::DeleatePlayer(ABattlePS* InBattlePS)
{
	if (nullptr == InBattlePS)
		return;

	for (int i = 0; i < PlayerListBox->GetChildrenCount(); i++)
	{
		UPlayerListItem_WS* PlayerListItem = Cast<UPlayerListItem_WS>(PlayerListBox->GetChildAt(i));
		if (PlayerListItem->BattlePS == InBattlePS)
		{
			PlayerListBox->RemoveChild(PlayerListItem);
			return;
		}
	}
}

void UPlayerList_WS::UpdateItemList()
{
	PlayerListBox->ClearChildren();
	FSoftObjectPath WidgetClassLoader(TEXT("WidgetBlueprint'/Game/UI/PlayerListItem_WB.PlayerListItem_WB_C'"));
	UClass* WidgetClass = Cast<UClass>(WidgetClassLoader.TryLoad());
	if (IsValid(WidgetClass))
	{
		UPlayerListItem_WS* NewPlayerListItem = CreateWidget<UPlayerListItem_WS>(GetOwningPlayer(), WidgetClass);
		if (IsValid(NewPlayerListItem))
		{
			PlayerListBox->AddChild(NewPlayerListItem);
		}
	}

	AGameStateBase* GameStateBase = UGameplayStatics::GetGameState(GetOwningPlayer()->GetWorld());
	for (auto PlayerState : GameStateBase->PlayerArray)
	{
		ABattlePS* BattlePS = Cast<ABattlePS>(PlayerState);
		if (IsValid(BattlePS))
		{
			AddPlayer(BattlePS);
		}
	}
}