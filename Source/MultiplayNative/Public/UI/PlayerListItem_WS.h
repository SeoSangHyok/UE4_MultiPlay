// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerListItem_WS.generated.h"

/**
 * 
 */
class UTextBlock;
class ABattlePS;

UCLASS()
class MULTIPLAYNATIVE_API UPlayerListItem_WS : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateItemList();
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathCount;

	UPROPERTY()
	ABattlePS* BattlePS;
};
