// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPC.generated.h"

/**
 * 
 */
class ULobbyWidget_WS;
class UInGameMenu_WS;

UCLASS()
class MULTIPLAYNATIVE_API ALobbyPC : public APlayerController
{
	GENERATED_BODY()
	
	
public:
	/** Transitions to calls BeginPlay on actors. */
	UFUNCTION(BlueprintCallable, Category = Game)
	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable)
	void Client_ShowMessage(const FString& Message);
	void Client_ShowMessage_Implementation(const FString& Message);

	UFUNCTION(Client, Reliable)
	void Client_RoomManagerElected();
	void Client_RoomManagerElected_Implementation();

// 	UFUNCTION(Client, Reliable)
// 	void Client_WarpRoom(int PortNo);
// 	void Client_WarpRoom_Implementation(int PortNo);

	UFUNCTION(Server, Reliable, WithValidation)
	void Serv_ShowMessage(const FString& Message);
	bool Serv_ShowMessage_Validate(const FString& Message) { return true; }
	void Serv_ShowMessage_Implementation(const FString& Message);

	UFUNCTION(Server, Reliable, WithValidation)
	void Serv_SendChatMessage(const FText& Message);
	bool Serv_SendChatMessage_Validate(const FText& Message) { return true; }
	void Serv_SendChatMessage_Implementation(const FText& Message);

	UFUNCTION(Server, Reliable, WithValidation)
	void Serv_SetPlayerName(const FString& InPlayerName);
	bool Serv_SetPlayerName_Validate(const FString& InPlayerName) { return true; }
	void Serv_SetPlayerName_Implementation(const FString& InPlayerName);

	UFUNCTION(Server, Reliable, WithValidation)
	void Serv_StartGame();
	bool Serv_StartGame_Validate() { return true; }
	void Serv_StartGame_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Serv_CreateRoom();
	bool Serv_CreateRoom_Validate() { return true; }
	void Serv_CreateRoom_Implementation();


	UFUNCTION(Server, Reliable, WithValidation)
	void Serv_JoineRoom(int RoomNo);
	bool Serv_JoineRoom_Validate(int RoomNo) { return true; }
	void Serv_JoineRoom_Implementation(int RoomNo);

	UFUNCTION(Client, Reliable)
	void Clnt_SendChatMessage(const FText& Message);
	void Clnt_SendChatMessage_Implementation(const FText& Message);

	UFUNCTION(Client, Reliable)
	void Clnt_ReadyToStartGame();
	void Clnt_ReadyToStartGame_Implementation();

	UFUNCTION(Client, Reliable)
	void Clnt_WarpRoom(int PortNo);
	void Clnt_WarpRoom_Implementation(int PortNo);

	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void OnInputEnter();

	UFUNCTION()
	void OnKeyInput_0();

	UFUNCTION()
	void OnKeyInput_1();

	UFUNCTION()
	void OnKeyInput_2();

	UFUNCTION()
	void OnKeyToggleMenu();

	UFUNCTION()
	void OnKeyInput(FKey Key);

	void SetRoomlistVisible(bool In_Visible);

protected:
	virtual void SetupInputComponent() override;

public:
	ULobbyWidget_WS* LobbyWidget;
	UInGameMenu_WS* InGameMenuWidget;
	bool IsRoomManager = false;
	int JoinedRoomNo = 0;
};
