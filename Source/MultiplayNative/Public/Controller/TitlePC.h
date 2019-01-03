// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TitlePC.generated.h"

/**
 * 
 */
class UUserWidget;

UCLASS()
class MULTIPLAYNATIVE_API ATitlePC : public APlayerController
{
	GENERATED_BODY()
	
	
public:
	virtual void BeginPlay() override;

	void MakeGame();
	void JoinGame(FName IpAddress, FString OptionString);

public:
	UUserWidget* TItleWidget;
};
