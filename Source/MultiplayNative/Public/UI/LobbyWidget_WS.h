// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget_WS.generated.h"

/**
 * 
 */
#define BATTLE_ENUM_STRING(BattleEnum) TEXT("BattleEnum")

class UScrollBox;
class UEditableTextBox;
class UButton;
class UVerticalBox;
class UBorder;

UCLASS()
class MULTIPLAYNATIVE_API ULobbyWidget_WS : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	virtual bool Initialize() override;

	UFUNCTION()
	void OnChatInputCommit(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void AddChatMessage(FText Message);

// 	UFUNCTION()
// 	void OnClickStart();

// 	UFUNCTION()
// 	void OnClickSocketConnect();
// 
// 	UFUNCTION()
// 	void OnClickSendDataSocket();

	UFUNCTION()
	void OnClickCreateRoomBtn();

	void SetFocusToEditBox();

	void SetHost(bool HostCheck);

	void SetRoomlistVisible(bool In_Visible);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UScrollBox*	ChatListScroll;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UEditableTextBox* ChatInput;

// 	UPROPERTY(meta = (BindWidget))
// 	UButton* StartGame;
// 
// 	UPROPERTY(meta = (BindWidget))
// 	UButton* SocketConnect;
// 
// 	UPROPERTY(meta = (BindWidget))
// 	UEditableTextBox* TestSocketIP;
// 
// 	UPROPERTY(meta = (BindWidget))
// 	UEditableTextBox* TestSocketPort;

// 	UPROPERTY(meta = (BindWidget))
// 	UEditableTextBox* SendText;

// 	UPROPERTY(meta = (BindWidget))
// 	UButton* DataSend;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* RoomListBox;

	UPROPERTY(meta = (BindWidget))
	UButton* CreateRoomBtn;

	UPROPERTY(meta = (BindWidget))
	UBorder* RoomMenu;

	bool IsHost = false;
};
