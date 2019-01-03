// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGS.h"




void ALobbyGS::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyGS, RoomList);
}

void ALobbyGS::OnRoomInfoChanged()
{

}

bool ALobbyGS::AddRoom(int RoomNo, int Port, TArray<FString>& PlayerList, EBattleState BatteState)
{
	FRoomInfo RoomInfo;
	if (GetRoomInfo(RoomNo, RoomInfo))
		return false;

	RoomInfo.RoomNo = RoomNo;
	RoomInfo.Port = Port;
	RoomInfo.PlayerList = PlayerList;
	RoomInfo.BattleState = BatteState;

	RoomList.Add(RoomInfo);

	return true;
}

bool ALobbyGS::ChangeRoomInfo(const FRoomInfo& RoomInfo)
{
	for (FRoomInfo& TargetRoomInfo : RoomList)
	{
		if (TargetRoomInfo.RoomNo == RoomInfo.RoomNo)
		{
			TargetRoomInfo.PlayerList = RoomInfo.PlayerList;
			TargetRoomInfo.BattleState = RoomInfo.BattleState;
			return true;
		}
	}

	return true;
}

void ALobbyGS::DeleteRoom(int RoomNo)
{
	int TargetIndex = INDEX_NONE;

	for (int i=0 ; i<RoomList.Num() ; i++)
	{
		if (RoomList[i].RoomNo == RoomNo)
		{
			TargetIndex = i;
			break;
		}
	}

	if (TargetIndex != INDEX_NONE)
	{
		RoomList.RemoveAt(TargetIndex);
	}
}

bool ALobbyGS::GetRoomInfo(int RoomNo, FRoomInfo& OutRoomInfo)
{
	for (FRoomInfo& RoomInfo : RoomList)
	{
		if (RoomNo == RoomInfo.RoomNo)
		{
			OutRoomInfo = RoomInfo;
			return true;
		}
	}

	return true;
}

bool ALobbyGS::GetPlayerJoinedRoomInfo(const FString& PlayerName, FRoomInfo& OutRoomInfo)
{
	for (FRoomInfo& RoomInfo : RoomList)
	{
		for (const FString& JoinedPlayer : RoomInfo.PlayerList)
		{
			if (JoinedPlayer == PlayerName)
			{
				OutRoomInfo = RoomInfo;
				return true;
			}
		}
	}

	return false;
}

bool ALobbyGS::FindPlayer(FString PlayerName)
{
	for (FRoomInfo& RoomInfo : RoomList)
	{
		for (const FString& JoinedPlayer : RoomInfo.PlayerList)
		{
			if (JoinedPlayer == PlayerName)
				return true;
		}
	}

	return false;
}