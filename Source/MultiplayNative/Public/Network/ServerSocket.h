// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "DelegateCombinations.h"
#include "ServerSocket.generated.h"

/**
 * 
 */
//General Log
DECLARE_LOG_CATEGORY_EXTERN(LogServerSocket, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecvDataFromClient, const FString&, DataString);

class FSocket;
class UWorld;

UCLASS()
class MULTIPLAYNATIVE_API UServerSocket : public UObject
{
	GENERATED_BODY()
	
public:
	bool CreateSocket(UWorld* World, const FString& SocketName, const FString& Ip, const int32 Port);
	void MainProc();
	void CloseSocket();
	void BroadcastToClients(FString& SendData);
	void SendDataToClient(FSocket* TargetClient, FString& SendData);

private:
	void CheckDisconnectedClients();
	void CheckNewClientConnection();
	void CheckRecvClientData();

	//Format String IP4 to number array 
	bool FormatIP4ToNumber(const FString& Ip, uint8(&Out)[4]);

	//StringFromBinaryArray 
	FString StringFromBinaryArray(TArray<uint8>& BinaryArray);

	void Log(FString LogMessage);

public:
	FOnRecvDataFromClient OnRecvDataFromClient;

private:
	FString Ip;
	int32 Port;
	FSocket* ServerSocket;
	TArray<FSocket*> ConnectedClientsSocket;
	FTimerHandle MainProcTimerHandle;

//	TCircularBuffer<uint8> RecvDataBuffer;
};
