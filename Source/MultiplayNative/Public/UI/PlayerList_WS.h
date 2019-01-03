// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerList_WS.generated.h"

/**
 * 
 */
class UVerticalBox;
class ABattlePS;

UCLASS()
class MULTIPLAYNATIVE_API UPlayerList_WS : public UUserWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;

public:
	void AddPlayer(ABattlePS* InBattlePS);
	void DeleatePlayer(ABattlePS* InBattlePS);
	void UpdateItemList();

public:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* PlayerListBox;
};
