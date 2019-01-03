// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerResultItem_WS.generated.h"


/**
 * 
 */
class  UTextBlock;
struct FPlayerResultItem;

UCLASS()
class MULTIPLAYNATIVE_API UPlayerResultItem_WS : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void  SetPlayerResult(const FPlayerResultItem* PlayerResultItem);

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Rank;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score;
};
