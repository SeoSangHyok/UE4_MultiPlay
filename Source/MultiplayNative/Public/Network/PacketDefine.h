// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Database/StructureDefines.h"
#include "PacketDefine.generated.h"

/**
 * 
 */
#define MAKE_PACKET_HEADER(MainType, SubType) FPacketHeaderType(static_cast<uint16>(MainType), static_cast<uint16>(SubType))

class FJsonObject;

UENUM()
enum class EPacketMainType : uint8
{
	Lobby,
	Battle,
	MainTypeCount
};

UENUM()
enum class EPacketLobbySubType : uint8
{
	CreateRoom,
	RoomInfoChange,
	DestroyRoom,
	LobbySubTypeCount
};

UENUM()
enum class EPacketBattleSubType : uint8
{
	UserLogout,
	BattleTypeCount
};

USTRUCT(BlueprintType)
struct FTestPacket
{
	GENERATED_USTRUCT_BODY()

public:
	FTestPacket();
	bool ParseJsonString(FString& JsonString);

public:
	FString StrVal;
	TArray<FString> StrArr;
	int IntVal;
	TArray<int> IntArr;
	FVector VectorVal;
	TArray<FVector> VectorArr;
};



USTRUCT(BlueprintType)
struct FPacketHeaderType
{
	GENERATED_USTRUCT_BODY()

public:
	FPacketHeaderType(uint16 Category1 = 0, uint16 Category2 = 0, uint16 Category3 = 0, uint16 Category4 = 0)
	{ 
		TypeArr[0] = Category1;	TypeArr[1] = Category2;	TypeArr[2] = Category3;	TypeArr[3] = Category4;
	}

	FPacketHeaderType(const FPacketHeaderType & rVal)
	{
		PacketData = rVal.PacketData;
	}

	static FString GetJsonHeaderString() { return TEXT("PacketHeader"); }
	TSharedPtr<FJsonObject> MakeJsonObject();
	bool ParseJsonObject(const TSharedPtr<FJsonObject>& JsonObject);

	friend bool operator>(const FPacketHeaderType& lVal, const FPacketHeaderType& rVal) { return lVal.PacketData > rVal.PacketData; }
	FPacketHeaderType& operator=(FPacketHeaderType&& lVal) { PacketData = lVal.PacketData; return *this; }
	FPacketHeaderType& operator=(const FPacketHeaderType& lVal) { PacketData = lVal.PacketData; return *this;	}
	friend bool operator==(const FPacketHeaderType& lVal, const FPacketHeaderType& rVal) { return lVal.PacketData == rVal.PacketData; }
	friend bool operator<(const FPacketHeaderType& lVal, const FPacketHeaderType& rVal) { return lVal.PacketData < rVal.PacketData; }
	friend uint32 GetTypeHash(const FPacketHeaderType& rVal) { return GetTypeHash(rVal.PacketData); }

public:
	union 
	{
		uint16 TypeArr[4];
		uint64 PacketData = 0;
	};
};

//////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FPacketRoomCreateComplete
{
	GENERATED_USTRUCT_BODY()

public:
	FPacketRoomCreateComplete() { ; }
	static FString GetJsonHeaderString() { return TEXT("RoomCreateComplete"); }
	TSharedPtr<FJsonObject> MakeJsonObject();
	bool ParseJsonObject(const TSharedPtr<FJsonObject>& JsonObject);

public:
	int RoomNo = 0;
};

//////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FPacketChageRoomInfo
{
	GENERATED_USTRUCT_BODY()

public:
	FPacketChageRoomInfo() { ; }
	static FString GetJsonHeaderString() { return TEXT("PacketChageRoomInfo"); }
	TSharedPtr<FJsonObject> MakeJsonObject();
	bool ParseJsonObject(const TSharedPtr<FJsonObject>& JsonObject);

public:
	int RoomNo = 0;
	int RoomState = 0;
	TArray<FString> PlayerList;
};

//////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FPacketDeleteRoom
{
	GENERATED_USTRUCT_BODY()

public:
	FPacketDeleteRoom() { ; }
	static FString GetJsonHeaderString() { return TEXT("PacketDeleteRoom"); }
	TSharedPtr<FJsonObject> MakeJsonObject();
	bool ParseJsonObject(const TSharedPtr<FJsonObject>& JsonObject);

public:
	int RoomNo = 0;
};

//////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FPacketBattleUserLogout
{
	GENERATED_USTRUCT_BODY()

public:
	FPacketBattleUserLogout() { ; }
	static FString GetJsonHeaderString() { return TEXT("PacketBattleUserLogout"); }
	TSharedPtr<FJsonObject> MakeJsonObject();
	bool ParseJsonObject(const TSharedPtr<FJsonObject>& JsonObject);

public:
	int RoomNo;
	FString LogoutUserName;
};

UCLASS()
class MULTIPLAYNATIVE_API UPacketDefine : public UObject
{
	GENERATED_BODY()
	
	
	
	
};
