// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StructureDefines.h"
#include "BattlePC.generated.h"

/**
 * 
 */
class UBattle_WS;
class ABattlePS;
class UInGameMenu_WS;

UCLASS()
class MULTIPLAYNATIVE_API ABattlePC : public APlayerController
{
	GENERATED_BODY()
	
	
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	UFUNCTION()
	void OnInputEnter();
	
	UFUNCTION()
	void OnKeyToggleMenu();

	UFUNCTION()
	void OnPawnIsDead();

public:
	UFUNCTION(Server, Reliable, WithValidation)
	void Serv_SendChatMessage(const FText& Message);
	bool Serv_SendChatMessage_Validate(const FText& Message) { return true; }
	void Serv_SendChatMessage_Implementation(const FText& Message);

	UFUNCTION(Server, Reliable, WithValidation)
	void Serv_SetPlayerName(const FString& InPlayerName);
	bool Serv_SetPlayerName_Validate(const FString& InPlayerName) { return true; }
	void Serv_SetPlayerName_Implementation(const FString& InPlayerName);

	UFUNCTION(Server, Reliable, WithValidation)
	void Serv_StartBattle();
	bool Serv_StartBattle_Validate() { return true; }
	void Serv_StartBattle_Implementation();

	UFUNCTION(Client, Reliable)
	void Clnt_SendChatMessage(const FText& Message);
	void Clnt_SendChatMessage_Implementation(const FText& Message);

	UFUNCTION(Client, Reliable)
	void Clnt_ReadyToStartGame();
	void Clnt_ReadyToStartGame_Implementation();

	UFUNCTION(Client, Reliable)
	void Clnt_PlayerLogin(const FString& PlayerName);
	void Clnt_PlayerLogin_Implementation(const FString& PlayerName);

	UFUNCTION(Client, Reliable)
	void Clnt_PlayerLogout(const FString& PlayerName);
	void Clnt_PlayerLogout_Implementation(const FString& PlayerName);

	UFUNCTION(Client, Reliable)
	void Clnt_NotifyPlayerDead(bool InDead);
	void Clnt_NotifyPlayerDead_Implementation(bool InDead);

	UFUNCTION(Client, Reliable)
	void Clnt_ShowGameResult(const TArray<FPlayerResultItem>& PlayerResults);
	void Clnt_ShowGameResult_Implementation(const TArray<FPlayerResultItem>& PlayerResults);

	UFUNCTION(Client, Reliable)
	void Clnt_RoomManagerElected();
	void Clnt_RoomManagerElected_Implementation();

	UFUNCTION(Client, Reliable)
	void Clnt_GotoLobby();
	void Clnt_GotoLobby_Implementation();

	UBattle_WS* GetBattleWidget() { return BattleWS; }

	void ShowPlayerList();
	void HIdePlayerList();
private:
	UPROPERTY()
	UBattle_WS* BattleWS;

	UPROPERTY()
	UInGameMenu_WS* InGameMenuWidget;

	UPROPERTY()
	FTimerHandle RespawnTimerHandler;

	int RemainRespawnTime;
	const int DefaultRespawnTime = 14;

	bool IsRoomManager = false;
};
