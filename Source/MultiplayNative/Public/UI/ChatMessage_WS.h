// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatMessage_WS.generated.h"

/**
 * 
 */
class UTextBlock;

UCLASS()
class MULTIPLAYNATIVE_API UChatMessage_WS : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual bool Initialize() override;

	UFUNCTION()
	void SetChatMessage(FText Message);

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ChatTextBlock;

	UPROPERTY()
	FText ChatMessage;
};
