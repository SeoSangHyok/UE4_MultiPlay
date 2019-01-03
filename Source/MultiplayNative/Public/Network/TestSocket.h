// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Networking/Public/Interfaces/IPv4/IPv4Endpoint.h"
#include "TestSocket.generated.h"

/**
 * 
 */
 //General Log
DECLARE_LOG_CATEGORY_EXTERN(LogNormal, Log, All);

class FSocket;
class UWorld;
struct FIPv4Endpoint;

UCLASS()
class MULTIPLAYNATIVE_API UTestSocket : public UObject
{
	GENERATED_BODY()

	~UTestSocket();
public:
	bool StartMyListenSocket(const FString& SocketName, const FString& Ip, const int32 Port);
	void CheckClientConnect();
	void CheckClinetSocketData();

	bool ConnectToServer(const FString& SocketName, const FString& Ip, const int32 Port);
	void CheckServerSocketData();

	bool StartTCPReceiver(const FString& SocketName, const FString& Ip, const int32 Port);
	FSocket* CreateTCPConnectionListener(const FString& SocketName, const FString& Ip, const int32 Port, const int32 RecvBufferSize = 2*1024*1024);

	//Timer functions, could be threads 
	void TCPConnectionListener();

	//can thread this eventually 
	void TCPSocketListener();

	//Format String IP4 to number array 
	bool FormatIP4ToNumber(const FString& Ip, uint8 (&out)[4]);

	//StringFromBinaryArray 
	FString StringFromBinaryArray(TArray<uint8>& BinaryArray);
	
public:
	FIPv4Endpoint MyServerRemoteAddressForConnection;
	FSocket* MyServerListenSocket;
	TArray<FSocket*> MyClientsSocket;

	FTimerHandle CheckClientConnectTimerHandle;
	FTimerHandle CheckClinetSocketDataTimerHandle;
	FTimerHandle CheckServerSocketDataTimerHandle;

	FSocket* ConnectedSocket;

	FSocket* ListenSocket;
	FIPv4Endpoint RemoteAddressForConnection;

	TArray<FSocket*> ConnectedSockets;

	FSocket* ConnectionSocket;

	UWorld* World;
	FTimerHandle ConnectionListenerTimerHandle;
	FTimerHandle SocketListenerTimerHandle;
};
