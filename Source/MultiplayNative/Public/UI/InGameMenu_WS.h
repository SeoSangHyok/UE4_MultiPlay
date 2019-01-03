// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenu_WS.generated.h"

/**
 * 
 */
class UButton;

UCLASS()
class MULTIPLAYNATIVE_API UInGameMenu_WS : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	UFUNCTION()
	void OnClickReturnToTitle();

	UFUNCTION()
	void OnClickExitGame();

public:
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ReturnToTitle;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ExitGame;
};
