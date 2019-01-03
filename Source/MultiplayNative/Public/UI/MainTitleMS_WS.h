// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainTitleMS_WS.generated.h"

/**
 * 
 */
class UButton;
class UEditableTextBox;

UCLASS(Config=Game)
class MULTIPLAYNATIVE_API UMainTitleMS_WS : public UUserWidget
{
	GENERATED_BODY()
	
	
	
public:
	virtual bool Initialize() override;


	UFUNCTION()
	void OnClickJoin();

	UFUNCTION()
	void OnClickExit();

	UFUNCTION()
	void OnUserNameInputCommit(const FText& Text, ETextCommit::Type CommitMethod);

public:
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Join;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Exit;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* UserNameInput;

	FName IpName;

	UPROPERTY(Config)
	FName TestIpName;

	UPROPERTY(Config)
	int TestPort;
};
