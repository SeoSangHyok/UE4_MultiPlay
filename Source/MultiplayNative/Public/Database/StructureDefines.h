// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnumDefines.h"
#include "StructureDefines.generated.h"

/**
 * 
 */

//enum class EBattleState : uint8;

USTRUCT(BlueprintType)
struct FPlayerResultItem
{
	GENERATED_USTRUCT_BODY()

public:
	FPlayerResultItem() 
	{
		PlayerName = TEXT("");
		Score = 0;
		Kill = 0;
		Death = 0;
		Rank = 0;
	}

	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	int Score;

	UPROPERTY()
	int Kill;

	UPROPERTY()
	int Death;

	UPROPERTY()
	int Rank;
};

USTRUCT(BlueprintType)
struct FRoomInfo
{
	GENERATED_USTRUCT_BODY()

public:
	FRoomInfo();

	UPROPERTY()
	int RoomNo;

	UPROPERTY()
	int Port;

	UPROPERTY()
	TArray<FString> PlayerList;

	UPROPERTY()
	EBattleState BattleState;
};

UCLASS()
class MULTIPLAYNATIVE_API UStructureDefines : public UObject
{
	GENERATED_BODY()
};


