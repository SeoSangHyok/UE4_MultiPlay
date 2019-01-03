// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StructureDefines.h"
#include "BattleGM.generated.h"

/**
 * 
 */
class ULobbyWidget_WS;
class ABattlePC;
class ABattleGS;
class UClientSocket;

enum class EBattleState : uint8;

UCLASS()
class MULTIPLAYNATIVE_API ABattleGM : public AGameModeBase
{
	GENERATED_BODY()



	ABattleGM();
	~ABattleGM();

public:
	virtual void StartPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	EBattleState GetBattleState() { return BattleState; }

	UFUNCTION()
	void MainTImer();

	UFUNCTION()
	void OnRecvDataFromServer(const FString& DataString);

	UFUNCTION()
	void AddPlayer(FString PlayerName);

	void RoomManagerElect();

	void StartBattle();

private:
	bool CheckPlayerJoinComplete();
	void CalcGameResult();

private:
	int PrevLevelNumPlayers = 0;

	UPROPERTY()
	ABattleGS* BattleGS;

	UPROPERTY()
	TArray<ABattlePC*> BattlePCs;

	UPROPERTY()
	FTimerHandle MainTimerHandle;

	EBattleState BattleState;

	int RemainPlayingTime;

	TArray<AActor*> PlayerStartList;
	TArray<FPlayerResultItem> PlayerResults;

	UPROPERTY()
	UClientSocket* SocketToMasterServ;

	UPROPERTY()
	ABattlePC* RoomManagerPC = nullptr;

	int RoomNo = 0;
};
