// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DelegateCombinations.h"
#include "ClientSocket.generated.h"

/**
 * 
 */
 //General Log
DECLARE_LOG_CATEGORY_EXTERN(LogClientSocket, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecvDataFromServer, const FString&, DataString);

class FSocket;
class UWorld;

UCLASS()
class MULTIPLAYNATIVE_API UClientSocket : public UObject
{
	GENERATED_BODY()
	
	
public:
	bool ConnectToServer(UWorld* World, const FString& SocketName, const FString& Ip, const int32 Port);
	void MainProc();
	void CloseSocket();
	void SendDataToServer(FString& SendData);
	void Log(FString LogMessage);

private:
	//Format String IP4 to number array 
	bool FormatIP4ToNumber(const FString& Ip, uint8(&Out)[4]);

	//StringFromBinaryArray 
	FString StringFromBinaryArray(TArray<uint8>& BinaryArray);

public:
	FOnRecvDataFromServer OnRecvDataFromServer;

private:
	FString Ip;
	int32 Port;
	FSocket* ClientSocket;
	FTimerHandle MainProcTimerHandle;
};
