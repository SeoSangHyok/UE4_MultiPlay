// Fill out your copyright notice in the Description page of Project Settings.

#include "TestSocket.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "../Networking/Public/Interfaces/IPv4/IPv4Endpoint.h"
#include "../Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Sockets.h"
#include "../Networking/Public/Common/TcpSocketBuilder.h"

// General Log
DEFINE_LOG_CATEGORY(LogNormal);


UTestSocket::~UTestSocket()
{
	for (FSocket* Socket : MyClientsSocket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
	}

	MyClientsSocket.Reset();

	if (nullptr != MyServerListenSocket)
	{
		MyServerListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(MyServerListenSocket);
	}

	if (nullptr != ConnectedSocket)
	{
		ConnectedSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectedSocket);
	}	
}

bool UTestSocket::StartMyListenSocket(const FString& SocketName, const FString& Ip, const int32 Port)
{
	uint8 Ip4Nums[4] = { 0, };
	if (!FormatIP4ToNumber(Ip, Ip4Nums))
	{
		UE_LOG(LogNormal, Warning, TEXT("Invalid IP! Expecting 4 parts separated by ."));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Invalid IP! Expecting 4 parts separated by .")));
		return nullptr;
	}

	//Create Socket
	FIPv4Endpoint IPv4Endpoint(FIPv4Address(Ip4Nums[0], Ip4Nums[1], Ip4Nums[2], Ip4Nums[3]), Port);
	MyServerListenSocket = FTcpSocketBuilder(SocketName).AsReusable().BoundToEndpoint(IPv4Endpoint).Listening(8);
	if (nullptr == MyServerListenSocket)
	{
		UE_LOG(LogNormal, Warning, TEXT("StartTCPReceiver>> Listen socket could not be created! ~> %s %d"), *Ip, Port);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("StartTCPReceiver>> Listen socket could not be created! ~> %s %d"), *Ip, Port));
		return false;
	}

	//Set Buffer Size
	int32 NewSize = 0;
	MyServerListenSocket->SetReceiveBufferSize((2 * 1024 * 1024), NewSize);

	//Start the Listener! //thread this eventually
	if (IsValid(World))
	{
		FTimerManager& WorldTimerManager = World->GetTimerManager();
		WorldTimerManager.SetTimer(CheckClientConnectTimerHandle, this, &UTestSocket::CheckClientConnect, 0.01f, true);
	}

	return true;
}

void UTestSocket::CheckClientConnect()
{
	if (nullptr == MyServerListenSocket)
	{
		return;
	}

	//Remote address
	TSharedRef<FInternetAddr> NewClientAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	// handle incoming connections
	bool Pending;
	if (MyServerListenSocket->HasPendingConnection(Pending) && Pending)
	{
		// Already have a Connection? destroy previous
		FSocket* NewClientSocket = MyServerListenSocket->Accept(*NewClientAddr, TEXT("Recv NewClientSocket Connected"));
		for (FSocket* Socket : MyClientsSocket)
		{
			TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
			Socket->GetAddress(*InternetAddr);
			if (InternetAddr->ToString(true) == NewClientAddr->ToString(true))
			{
				Socket->Close();
				ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
				MyClientsSocket.Remove(Socket);
				break;
			}
		}

		MyClientsSocket.Add(NewClientSocket);

		UE_LOG(LogNormal, Warning, TEXT("Accepted Connection! WOOOHOOOO!!!"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Accepted Connection! WOOOHOOOO!!!")));
		if (IsValid(World))
		{
			FTimerManager& WorldTimerManager = World->GetTimerManager();
			WorldTimerManager.SetTimer(CheckClinetSocketDataTimerHandle, this, &UTestSocket::CheckClinetSocketData, 0.01f, true);
		}
	}
}

void UTestSocket::CheckClinetSocketData()
{
	for (FSocket* Socket : MyClientsSocket)
	{
		if (Socket->GetConnectionState() != SCS_Connected)
		{
			Socket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
			MyClientsSocket.Remove(Socket);
			continue;
		}

		TArray<uint8> ReceivedData;
		uint32 ReceivedDataSize = 0;
		while (Socket->HasPendingData(ReceivedDataSize))
		{
			ReceivedData.SetNumZeroed(FMath::Min(ReceivedDataSize, 65507u));
			int32 Read = 0;
			Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
			UE_LOG(LogNormal, Warning, TEXT("Data Read! %d"), ReceivedData.Num());
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Read! %d"), ReceivedData.Num()));
		}

		if (ReceivedData.Num() <= 0)
		{
			continue;
		}

		UE_LOG(LogNormal, Warning, TEXT("Data Bytes Read ~> %d"), ReceivedData.Num());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Bytes Read ~> %d"), ReceivedData.Num()));

		const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
		UE_LOG(LogNormal, Warning, TEXT("As String Data ~> %s"), *ReceivedUE4String);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("As String Data ~> %s"), *ReceivedUE4String));
	}
}

bool UTestSocket::ConnectToServer(const FString& SocketName, const FString& Ip, const int32 Port)
{
	uint8 Ip4Nums[4] = { 0, };
	if (!FormatIP4ToNumber(Ip, Ip4Nums))
	{
		UE_LOG(LogNormal, Warning, TEXT("Invalid IP! Expecting 4 parts separated by ."));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Invalid IP! Expecting 4 parts separated by .")));
		return false;
	}

	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	if (nullptr == SocketSubsystem)
	{
		UE_LOG(LogNormal, Warning, TEXT("Getting SocketSubsystem Fail : ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Getting SocketSubsystem Fail : ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);")));
		return false;
	}
	
	ConnectedSocket = SocketSubsystem->CreateSocket(NAME_Stream, *SocketName, true);
	if (nullptr == ConnectedSocket)
	{
		UE_LOG(LogNormal, Warning, TEXT("Getting SocketSubsystem Fail : ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Getting SocketSubsystem Fail : ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);")));
		return false;
	}

	FIPv4Address ServIP(Ip4Nums[0], Ip4Nums[1], Ip4Nums[2], Ip4Nums[3]);
	TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
	Addr->SetIp(ServIP.Value);
	Addr->SetPort(Port);

	if (ConnectedSocket->Connect(*Addr) == false)
	{
		ConnectedSocket->Close();
		SocketSubsystem->DestroySocket(ConnectedSocket);
		ConnectedSocket = nullptr;

		UE_LOG(LogNormal, Warning, TEXT("Server Connect Fail"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Server Connect Fail")));
		return false;
	}

	if (IsValid(World))
	{
		FTimerManager& WorldTimerManager = World->GetTimerManager();
		WorldTimerManager.SetTimer(CheckServerSocketDataTimerHandle, this, &UTestSocket::CheckServerSocketData, 0.01f, true);
	}

	return true;
}

void UTestSocket::CheckServerSocketData()
{
	TArray<uint8> ReceivedData;
	uint32 ReceivedDataSize = 0;
	while (ConnectedSocket->HasPendingData(ReceivedDataSize))
	{
		ReceivedData.SetNumZeroed(FMath::Min(ReceivedDataSize, 65507u));
		int32 Read = 0;
		ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
		UE_LOG(LogNormal, Warning, TEXT("Data Read! %d"), ReceivedData.Num());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CheckServerSocketData Data Read! %d"), ReceivedData.Num()));
	}

	if (ReceivedData.Num() <= 0)
	{
		return;
	}

	UE_LOG(LogNormal, Warning, TEXT("Data Bytes Read ~> %d"), ReceivedData.Num());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Bytes Read ~> %d"), ReceivedData.Num()));

	const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
	UE_LOG(LogNormal, Warning, TEXT("As String Data ~> %s"), *ReceivedUE4String);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("As String Data ~> %s"), *ReceivedUE4String));
}

bool UTestSocket::StartTCPReceiver(const FString& SocketName, const FString& Ip, const int32 Port)
{
	//Rama's CreateTCPConnectionListener
	ListenSocket = CreateTCPConnectionListener(SocketName, Ip, Port);

	//Not created?
	if (!ListenSocket)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("StartTCPReceiver>> Listen socket could not be created! ~> %s %d"), *Ip, Port));
		return false;
	}

	//Start the Listener! //thread this eventually
	if (IsValid(World))
	{
		FTimerManager& WorldTimerManager = World->GetTimerManager();
		WorldTimerManager.SetTimer(ConnectionListenerTimerHandle, this, &UTestSocket::TCPConnectionListener, 0.01f, true);
	}

	return true;
}

FSocket* UTestSocket::CreateTCPConnectionListener(const FString& SocketName, const FString& Ip, const int32 Port, const int32 RecvBufferSize/* = 2 * 1024 * 1024*/)
{
	uint8 Ip4Nums[4] = { 0, };
	if (!FormatIP4ToNumber(Ip, Ip4Nums))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Invalid IP! Expecting 4 parts separated by .")));
		return nullptr;
	}

	//Create Socket
	FIPv4Endpoint IPv4Endpoint(FIPv4Address(Ip4Nums[0], Ip4Nums[1], Ip4Nums[2], Ip4Nums[3]), Port);
	FSocket* ListenerSocket = FTcpSocketBuilder(SocketName).AsReusable().BoundToEndpoint(IPv4Endpoint).Listening(8);

	//Set Buffer Size
	int32 NewSize = 0;
	ListenerSocket->SetReceiveBufferSize(RecvBufferSize, NewSize);

	return ListenerSocket;
}

void UTestSocket::TCPConnectionListener()
{
	if (nullptr == ListenSocket)
	{
		return;
	}

	//Remote address
	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	// handle incoming connections
	bool Pending;
	if (ListenSocket->HasPendingConnection(Pending) && Pending)
	{
		//Already have a Connection? destroy previous
		if (nullptr != ConnectionSocket)
		{
			ConnectionSocket->Close();
			ConnectionSocket = nullptr;
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		}

		//New Connection receive!
		ConnectionSocket = ListenSocket->Accept(*RemoteAddress, TEXT("Recv Socket Connection"));

		if (nullptr != ConnectionSocket)
		{
			//Global cache of current Remote Address
			RemoteAddressForConnection = FIPv4Endpoint(RemoteAddress);
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Accepted Connection! WOOOHOOOO!!!")));
		if (IsValid(World))
		{
			FTimerManager& WorldTimerManager = World->GetTimerManager();
			WorldTimerManager.SetTimer(SocketListenerTimerHandle, this, &UTestSocket::TCPSocketListener, 0.01f, true);
		}
	}
}

void UTestSocket::TCPSocketListener()
{
	if (nullptr == ConnectionSocket)
	{
		return;
	}

	TArray<uint8> ReceivedData;

	uint32 Size = 0;
	while (ConnectionSocket->HasPendingData(Size))
	{
		ReceivedData.SetNumZeroed(FMath::Min(Size, 65507u));
		int32 Read = 0;
		ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Read! %d"), ReceivedData.Num()));
	}

	if (ReceivedData.Num() <= 0)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Bytes Read ~> %d"), ReceivedData.Num()));

	const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("As String Data ~> %s"), *ReceivedUE4String));
}

bool UTestSocket::FormatIP4ToNumber(const FString& Ip, uint8(&Out)[4])
{
	Ip.Replace(TEXT(" "), TEXT(""));

	TArray<FString> Parts;
	Ip.ParseIntoArray(Parts, TEXT("."), true);
	if (Parts.Num() != 4)
		return false;

	for (int32 i = 0; i < 4; ++i)
	{
		Out[i] = FCString::Atoi(*Parts[i]);
	}

	return true;
}

FString UTestSocket::StringFromBinaryArray(TArray<uint8>& BinaryArray)
{
	BinaryArray.Add(0);
	// Add 0 termination. Even if the string is already 0-terminated, it doesn't change the results.
	// Create a string from a byte array. The string is expected to be 0 terminated (i.e. a byte set to 0).
	// Use UTF8_TO_TCHAR if needed.
	// If you happen to know the data is UTF-16 (USC2) formatted, you do not need any conversion to begin with.
	// Otherwise you might have to write your own conversion algorithm to convert between multilingual UTF-16 planes.
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}