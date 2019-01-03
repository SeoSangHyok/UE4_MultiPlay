// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PacketProcessor.h"
#include "PacketDefine.h"
#include <map>
#include "LobbyGM.generated.h"

/**
 * 
 */
//#define MAX_ROOM_COUNT 100

class UServerSocket;
class UPacketProcessor;
class FJsonObject;

UCLASS()
class MULTIPLAYNATIVE_API ALobbyGM : public AGameModeBase
{
	GENERATED_BODY()
	
	typedef void (ALobbyGM::*PacketProcCallback)(const TSharedPtr<FJsonObject>& JsonObject);

public:
	ALobbyGM();

	/** Transitions to calls BeginPlay on actors. */
	UFUNCTION(BlueprintCallable, Category = Game)
	virtual void StartPlay() override;

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

//	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
//	void RoomManagerElect();

	void CreateBattleRoom(APlayerController* RequestPC);

	bool FIndPlayer(const FString& PlayerName);

	UFUNCTION()
	void OnRecvDataFromClient(const FString& DataString);

	void OnLobby_CreateRoomComplete(const TSharedPtr<FJsonObject>& JsonObject);
	void OnLobby_RoomInfoChange(const TSharedPtr<FJsonObject>& JsonObject);
	void OnLobby_DestroyRoom(const TSharedPtr<FJsonObject>& JsonObject);
	void OnBattle_UserLogout(const TSharedPtr<FJsonObject>& JsonObject);



private:
	enum
	{
		MAX_ROOM_COUNT = 100,
	};

	TArray<APlayerController*> PlayerControllerArr;

	UPROPERTY()
	UServerSocket* ServerSocket;

	int NextRoomPortIdx = 0;
	int NextRoomNumber = 1;
	bool PortEnable[MAX_ROOM_COUNT];
	TMap<FPacketHeaderType, PacketProcCallback> PacketProcCallbackMap;
};
