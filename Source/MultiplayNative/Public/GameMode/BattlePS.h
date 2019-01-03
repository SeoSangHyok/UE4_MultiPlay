// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BattlePS.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYNATIVE_API ABattlePS : public APlayerState
{
	GENERATED_BODY()
	
	
protected:
	/** Copy properties which need to be saved in inactive PlayerState */
	virtual void CopyProperties(APlayerState* PlayerState) override;

public:
//	UPROPERTY(ReplicatedUsing = Rep_HP)
	UPROPERTY(Replicated)
	int KillCount = 0;

	UPROPERTY(Replicated)
	int DeathCount = 0;
};
