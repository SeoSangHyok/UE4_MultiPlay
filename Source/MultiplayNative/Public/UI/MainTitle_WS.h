// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainTitle_WS.generated.h"

/**
 * 
 */
class UButton;
class UEditableTextBox;

UCLASS()
class MULTIPLAYNATIVE_API UMainTitle_WS : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	virtual bool Initialize() override;


	UFUNCTION()
	void OnClickCreateRoom();

	UFUNCTION()
	void OnClickJoin();

	UFUNCTION()
	void OnClickExit();

	UFUNCTION()
	void OnUserNameInputCommit(const FText& Text, ETextCommit::Type CommitMethod);

public:
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_CreateRoom;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Join;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Exit;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* IPAddress;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* UserNameInput;
};
