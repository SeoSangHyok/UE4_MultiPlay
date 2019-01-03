// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BattleGS.generated.h"

/**
 * 
 */
class ABattlePS;

UCLASS()
class MULTIPLAYNATIVE_API ABattleGS : public AGameStateBase
{
	GENERATED_BODY()



	UFUNCTION()
	void Rep_MainMessage();

protected:
	virtual void BeginPlay();

public:
	ABattlePS* GetPlayerState(const FString& PlayerName);

public:
	UPROPERTY(ReplicatedUsing = Rep_MainMessage)
	FString MainMessage;

//	UPROPERTY(ReplicatedUsing = Rep_PlayerList)
	TArray<FString> PlayerList;

	UPROPERTY(Replicated)
	int RoomNo;

	UPROPERTY(Replicated)
	EBattleState BattleState;
};
