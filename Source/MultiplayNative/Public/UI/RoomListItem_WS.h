// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomListItem_WS.generated.h"

/**
 * 
 */
class UTextBlock;
class UButton;

UCLASS()
class MULTIPLAYNATIVE_API URoomListItem_WS : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual bool Initialize() override;

	void SetInfo(int RoomNo, int PlayerCount, FString PlayState);

	UFUNCTION()
	void OnClickJoin();

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoomNoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock*	PlayerCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock*	StateText;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinBtn;

private:
	int RoomID;
};
