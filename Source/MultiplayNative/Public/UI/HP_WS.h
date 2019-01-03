// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HP_WS.generated.h"

/**
 * 
 */
class  UTextBlock;

UCLASS()
class MULTIPLAYNATIVE_API UHP_WS : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	void SetHP(float hp);
	
public:
	UPROPERTY()
	UTextBlock* HP_Text;
};
