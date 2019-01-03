// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyWidget_WS.h"
#include "ScrollBox.h"
#include "EditableTextBox.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WidgetBlueprintLibrary.h"
#include "ChatMessage_WS.h"
#include "LobbyPC.h"
#include "Button.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayNativeGI.h"
#include "VerticalBox.h"
#include "Border.h"
#include "LobbyGS.h"
#include "RoomListItem_WS.h"


bool ULobbyWidget_WS::Initialize()
{
	Super::Initialize();

	ChatInput->OnTextCommitted.AddDynamic(this, &ULobbyWidget_WS::OnChatInputCommit);
	ChatInput->ClearKeyboardFocusOnCommit = false;

	if (IsValid(CreateRoomBtn))
	{
		CreateRoomBtn->OnClicked.AddDynamic(this, &ULobbyWidget_WS::OnClickCreateRoomBtn);
	}

	if (IsValid(RoomMenu))
	{
		RoomMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	return true;
}

void ULobbyWidget_WS::OnChatInputCommit(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (ETextCommit::Type::OnEnter == CommitMethod)
	{
		if (Text.IsEmpty())
		{
			UWidgetBlueprintLibrary::SetFocusToGameViewport();
		}
		else
		{
			ALobbyPC* LobbyPC = Cast<ALobbyPC>(GetOwningPlayer());
			LobbyPC->Serv_SendChatMessage(Text);
			ChatInput->SetText(FText::GetEmpty());
		}
	}
}

void ULobbyWidget_WS::AddChatMessage(FText Message)
{
	FSoftObjectPath WidgetLoader(TEXT("WidgetBlueprint'/Game/UI/ChatMessage_WB.ChatMessage_WB_C'"));
	UClass* WidgetClass = Cast<UClass>(WidgetLoader.TryLoad());

	UChatMessage_WS* ChatMessage_WS = CreateWidget<UChatMessage_WS>(GetOwningPlayer(), WidgetClass);
	ChatMessage_WS->SetChatMessage(Message);

	ChatListScroll->AddChild(ChatMessage_WS);
}

// void ULobbyWidget_WS::OnClickStart()
// {
// 	ALobbyPC* LobbyPC = Cast<ALobbyPC>(GetOwningPlayer());
// 	if (IsValid(LobbyPC))
// 	{
// 		LobbyPC->StartGame();
// 	}
// }

// void ULobbyWidget_WS::OnClickSocketConnect()
// {
// // 	if (!IsValid(ClientSocket))
// // 	{
// // 		ClientSocket = NewObject<UClientSocket>();
// // 		ClientSocket->ConnectToServer(GetOwningPlayer()->GetWorld(), TEXT("MultiplayNative Clnt Socket"), TestSocketIP->GetText().ToString(), FCString::Atoi(*TestSocketPort->GetText().ToString()));
// // 	}
// 
// 	
// 
// // 	UMultiplayNativeGI* MultiplayNativeGI = Cast<UMultiplayNativeGI>(UGameplayStatics::GetGameInstance(GetOwningPlayer()->GetWorld()));
// // 	if (IsValid(MultiplayNativeGI))
// // 	{
// // 		UTestSocket* TestSocket = MultiplayNativeGI->TestSocket;
// // 		TestSocket->ConnectToServer(TEXT("MultiplayNative Clnt Socket"), TestSocketIP->GetText().ToString(), FCString::Atoi(*TestSocketPort->GetText().ToString()));
// // 	}
// 
// }

void ULobbyWidget_WS::OnClickCreateRoomBtn()
{
	ALobbyPC* LobbyPC = Cast<ALobbyPC>(GetOwningPlayer());
	LobbyPC->Serv_CreateRoom();
}

void ULobbyWidget_WS::SetFocusToEditBox()
{
	ChatInput->SetKeyboardFocus();
}

void ULobbyWidget_WS::SetHost(bool HostCheck)
{
// 	IsHost = HostCheck;
// 	if (IsHost)
// 	{
// 		StartGame->SetVisibility(ESlateVisibility::Visible);
// 	}
// 	else
// 	{
// 		StartGame->SetVisibility(ESlateVisibility::Collapsed);
// 	}
}

void ULobbyWidget_WS::SetRoomlistVisible(bool In_Visible)
{
	if (true == In_Visible)
	{
		RoomListBox->ClearChildren();

		ALobbyGS* LobbyGS = Cast<ALobbyGS>(UGameplayStatics::GetGameState(GetOwningPlayer()->GetWorld()));
		FSoftObjectPath WidgetLoader(TEXT("WidgetBlueprint'/Game/UI/RoomListItem_WB.RoomListItem_WB_C'"));
		UClass* WidgetBpClass = Cast<UClass>(WidgetLoader.TryLoad());
		for (FRoomInfo& RoomInfo : LobbyGS->RoomList)
		{
			URoomListItem_WS* RoomListItem_WS = CreateWidget<URoomListItem_WS>(GetOwningPlayer(), WidgetBpClass);
			if (IsValid(RoomListItem_WS))
			{
				FString StatusString;
				switch (RoomInfo.BattleState)
				{
				case EBattleState::Createing:		StatusString = TEXT("Createing");		break;
				case EBattleState::Finish:			StatusString = TEXT("Finish");			break;
				case EBattleState::Playing:			StatusString = TEXT("Playing");			break;
				case EBattleState::WaitingPlayers:	StatusString = TEXT("WaitingPlayers");	break;
				}

				RoomListItem_WS->SetInfo(RoomInfo.RoomNo, RoomInfo.PlayerList.Num(), StatusString);

				RoomListBox->AddChild(RoomListItem_WS);
			}
		}
		


// 
// 		LobbyWidget = CreateWidget<ULobbyWidget_WS>(this, WidgetBpClass);
// 		if (IsValid(LobbyWidget))
// 		{
// 			LobbyWidget->AddToViewport();
// 		}


		RoomMenu->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		RoomMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
}