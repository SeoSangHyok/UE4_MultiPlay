// Fill out your copyright notice in the Description page of Project Settings.

#include "Battle_WS.h"
#include "ScrollBox.h"
#include "TextBlock.h"
#include "EditableTextBox.h"
#include "Image.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WidgetBlueprintLibrary.h"
#include "ChatMessage_WS.h"
#include "BattlePC.h"
#include "BattleGS.h"
#include "PlayerList_WS.h"
#include "BattleResult_WS.h"
#include "Button.h"
#include "Engine/Engine.h"


bool UBattle_WS::Initialize()
{
	Super::Initialize();

	ChatInput->OnTextCommitted.AddDynamic(this, &UBattle_WS::OnChatInputCommit);
	ChatInput->ClearKeyboardFocusOnCommit = false;

	if (IsValid(StartBtn))
	{
		StartBtn->OnClicked.AddDynamic(this, &UBattle_WS::OnClickedBattleStart);
		StartBtn->SetVisibility(ESlateVisibility::Collapsed);
	}

	PlayerList->SetVisibility(ESlateVisibility::Collapsed);
	BattleResult->SetVisibility(ESlateVisibility::Collapsed);
	SetRespawnMessage(TEXT(""));
	return true;
}

void UBattle_WS::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsValid(BattleGS))
	{
		SetMainMessage(BattleGS->MainMessage);
	}
}

void UBattle_WS::SetMainMessage(FString Message)
{
	if (IsValid(MainMessage))
	{
		MainMessage->SetText(FText::FromString(Message));
	}
}

void UBattle_WS::SetRespawnMessage(FString Message)
{
	if (IsValid(RespawnMessage))
	{
		RespawnMessage->SetText(FText::FromString(Message));
	}
}

void UBattle_WS::OnChatInputCommit(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (ETextCommit::Type::OnEnter == CommitMethod)
	{
		if (Text.IsEmpty())
		{
			UWidgetBlueprintLibrary::SetFocusToGameViewport();
		}
		else
		{
			ABattlePC* BattlePC = Cast<ABattlePC>(GetOwningPlayer());
			BattlePC->Serv_SendChatMessage(Text);
			ChatInput->SetText(FText::GetEmpty());
		}
	}
}

void UBattle_WS::OnClickedBattleStart()
{
	ABattlePC* BattlePC = Cast<ABattlePC>(GetOwningPlayer());
	if (IsValid(BattlePC))
	{
		BattlePC->Serv_StartBattle();
	}
}

void UBattle_WS::AddChatMessage(FText Message)
{
	FSoftObjectPath WidgetLoader(TEXT("WidgetBlueprint'/Game/UI/ChatMessage_WB.ChatMessage_WB_C'"));
	UClass* WidgetClass = Cast<UClass>(WidgetLoader.TryLoad());

	UChatMessage_WS* ChatMessage_WS = CreateWidget<UChatMessage_WS>(GetOwningPlayer(), WidgetClass);
	ChatMessage_WS->SetChatMessage(Message);

	ChatListScroll->AddChild(ChatMessage_WS);
}

void UBattle_WS::SetFocusToEditBox()
{
	ChatInput->SetKeyboardFocus();
}

void UBattle_WS::ShowPlayerList()
{
	PlayerList->SetVisibility(ESlateVisibility::Visible);
	PlayerList->UpdateItemList();
}

void UBattle_WS::HidePlayerList()
{
	PlayerList->SetVisibility(ESlateVisibility::Collapsed);
}

void UBattle_WS::SetVIsibleFireDot(bool InVisble)
{
	if (IsValid(FireDot))
	{
		if (true == InVisble)
		{
			FireDot->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			FireDot->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UBattle_WS::SetVisibleStartBattleButton(bool InVisible)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UBattle_WS::SetVisibleStartBattleButton"));

	if (IsValid(StartBtn))
	{
		if (InVisible)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("StartBtn->SetVisibility(ESlateVisibility::Visible);"));
			StartBtn->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("StartBtn->SetVisibility(ESlateVisibility::Collapsed);"));
			StartBtn->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}