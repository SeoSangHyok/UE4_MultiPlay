// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Battle_WS.generated.h"

/**
 * 
 */
class UScrollBox;
class UEditableTextBox;
class UTextBlock;
class UImage;
class UButton;
class ABattleGS;
class UPlayerList_WS;
class UBattleResult_WS;

UCLASS()
class MULTIPLAYNATIVE_API UBattle_WS : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void SetMainMessage(FString Message);

	UFUNCTION()
	void SetRespawnMessage(FString Message);

	UFUNCTION()
	void OnChatInputCommit(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnClickedBattleStart();

	UFUNCTION()
	void AddChatMessage(FText Message);

	void SetFocusToEditBox();

	void ShowPlayerList();
	void HidePlayerList();
	void SetVIsibleFireDot(bool InVisble);

	void SetVisibleStartBattleButton(bool InVisible);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* MainMessage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* RespawnMessage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UScrollBox*	ChatListScroll;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UEditableTextBox* ChatInput;

	UPROPERTY(meta = (BindWidget))
	UImage* FireDot;

	UPROPERTY(meta = (BindWidget))
	UButton* StartBtn;

	UPROPERTY(meta = (BindWidget))
	UPlayerList_WS* PlayerList;

	UPROPERTY(meta = (BindWidget))
	UBattleResult_WS* BattleResult;

	UPROPERTY()
	ABattleGS* BattleGS;
};
