// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DelegateCombinations.h"
#include "PacketProcessor.generated.h"

/**
 * 
 */
class FJsonObject;

USTRUCT(BlueprintType)
struct FParsedDataInfo
{
	GENERATED_USTRUCT_BODY()

public:
	FParsedDataInfo() { ; }

	TSharedPtr<FJsonObject> PacketHaderJsonObj;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPacketParseProc, FParsedDataInfo&, ParsedDataInfo);

UCLASS()
class MULTIPLAYNATIVE_API UPacketProcessor : public UObject
{
	GENERATED_BODY()

	UPacketProcessor();
	
	
public:
	UFUNCTION()
	void ParsePacketData(const FString& DataString);

public:
	TArray<TArray<FPacketParseProc>> PacketParseProcMap;
};
