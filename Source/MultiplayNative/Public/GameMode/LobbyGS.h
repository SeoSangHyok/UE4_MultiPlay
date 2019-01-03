// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "StructureDefines.h"
#include "LobbyGS.generated.h"

/**
 * 
 */
//struct FRoomInfo;

UCLASS()
class MULTIPLAYNATIVE_API ALobbyGS : public AGameStateBase
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION()
	void OnRoomInfoChanged();

	bool AddRoom(int RoomNo, int Port, TArray<FString>& PlayerList, EBattleState BatteState );
	bool ChangeRoomInfo(const FRoomInfo& RoomInfo);
	void DeleteRoom(int RoomNo);
	bool GetRoomInfo(int RoomNo, FRoomInfo& OutRoomInfo);
	bool GetPlayerJoinedRoomInfo(const FString& PlayerName, FRoomInfo& OutRoomInfo);
	bool FindPlayer(FString PlayerName);
// 	void JoinPlayerToRoom(int RoomNo, const FString& PlayerName);
// 	void DeletePlayerToRoom(int RoomNo, const FString& PlayerName);

public:
	//	UPROPERTY()
	UPROPERTY(ReplicatedUsing = OnRoomInfoChanged)
	TArray<FRoomInfo> RoomList;
};
