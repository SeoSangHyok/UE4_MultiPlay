// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomListItem_WS.h"
#include "TextBlock.h"
#include "Button.h"
#include "LobbyPC.h"




bool URoomListItem_WS::Initialize()
{
	Super::Initialize();

	return true;
}

void URoomListItem_WS::SetInfo(int RoomNo, int PlayerCount, FString PlayState)
{
	RoomID = RoomNo;
	RoomNoText->SetText(FText::FromString(FString::FromInt(RoomNo)));
	PlayerCountText->SetText(FText::FromString(FString::FromInt(PlayerCount)));
	StateText->SetText(FText::FromString(PlayState));

	if (IsValid(JoinBtn))
	{
		JoinBtn->OnClicked.AddDynamic(this, &URoomListItem_WS::OnClickJoin);
	}
}

void URoomListItem_WS::OnClickJoin()
{
	ALobbyPC* LobbyPC = Cast<ALobbyPC>(GetOwningPlayer());
	LobbyPC->Serv_JoineRoom(RoomID);
}