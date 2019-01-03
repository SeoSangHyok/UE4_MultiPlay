// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BattleWaitGS.generated.h"

/**
 * 
 */
struct FRoomInfo;

UCLASS()
class MULTIPLAYNATIVE_API ABattleWaitGS : public AGameStateBase
{
	GENERATED_BODY()
	
	
	
public:
	//UPROPERTY(Replicated)
	UPROPERTY()
	TArray<FRoomInfo> RoomList;
};
