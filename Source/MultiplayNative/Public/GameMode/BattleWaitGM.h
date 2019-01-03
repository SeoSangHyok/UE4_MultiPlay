// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PacketProcessor.h"
#include "PacketDefine.h"
#include <map>
#include "BattleWaitGM.generated.h"

/**
 * 
 */
//#define MAX_ROOM_COUNT 100

class UServerSocket;
class UPacketProcessor;
class FJsonObject;

UCLASS()
class MULTIPLAYNATIVE_API ABattleWaitGM : public AGameModeBase
{
	GENERATED_BODY()
	
	
	
	typedef void (ABattleWaitGM::*PacketProcCallback)(const TSharedPtr<FJsonObject>& JsonObject);

public:
	ABattleWaitGM();

	/** Transitions to calls BeginPlay on actors. */
	UFUNCTION(BlueprintCallable, Category = Game)
	virtual void StartPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

//	void RoomManagerElect();

//	void CreateBattleRoom(APlayerController* RequestPC);

	UFUNCTION()
	void OnRecvDataFromClient(const FString& DataString);

	void OnLobby_CreateRoomComplete(const TSharedPtr<FJsonObject>& JsonObject);

private:
	APlayerController* RoomManagerPC;
	TArray<APlayerController*> PlayerControllerArr;

	UPROPERTY()
	UServerSocket* ServerSocket;

	UPROPERTY()
	UPacketProcessor* PacketProcessor;

	int NextRoomPortIdx = 0;
	int NextRoomNumber = 1;
	bool PortEnable[100];
	TMap<FPacketHeaderType, PacketProcCallback> PacketProcCallbackMap;
};
