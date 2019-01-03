// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TitleGM.generated.h"

/**
 * 
 */
 //General Log
DECLARE_LOG_CATEGORY_EXTERN(LogTitleGM, Log, All);

class UUserWidget;

UCLASS()
class MULTIPLAYNATIVE_API ATitleGM : public AGameModeBase
{
	GENERATED_BODY()
	
	
public:
	ATitleGM();

	/** Transitions to calls BeginPlay on actors. */
	UFUNCTION(BlueprintCallable, Category = Game)
	virtual void StartPlay() override;
	
public:
	bool UseMasterServMode = true;

private:
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	UUserWidget* TItleWidget;
};
