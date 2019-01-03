// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StructureDefines.h"
#include "BattleResult_WS.generated.h"

/**
 * 
 */
class UVerticalBox;

UCLASS()
class MULTIPLAYNATIVE_API UBattleResult_WS : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	void MakeBattleResult(const TArray<FPlayerResultItem>& PlayerResults);
	
public:
	UPROPERTY(meta= (BindWidget))
	UVerticalBox* ResultListBox;
};
