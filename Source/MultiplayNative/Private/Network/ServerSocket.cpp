// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerSocket.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "../Networking/Public/Interfaces/IPv4/IPv4Endpoint.h"
#include "../Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "../Networking/Public/Common/TcpSocketBuilder.h"
#include "TimerManager.h"
#include "SocketSubsystem.h"


DEFINE_LOG_CATEGORY(LogServerSocket);


bool UServerSocket::CreateSocket(UWorld* World, const FString& SocketName, const FString& Ip, const int32 Port)
{
	if (nullptr != ServerSocket)
	{
		Log(FString::Printf(TEXT("Socket Already Created. Try After CloseSocket().")));
		return false;
	}

	uint8 Ip4Nums[4] = { 0, };
	if (!FormatIP4ToNumber(Ip, Ip4Nums))
	{
		Log(FString::Printf(TEXT("Invalid IP! Expecting 4 parts separated by . IP : %s"), *Ip));
		return nullptr;
	}

	//Create Socket
	FIPv4Endpoint IPv4Endpoint(FIPv4Address(Ip4Nums[0], Ip4Nums[1], Ip4Nums[2], Ip4Nums[3]), Port);
	ServerSocket = FTcpSocketBuilder(SocketName).AsReusable().BoundToEndpoint(IPv4Endpoint).Listening(8);
	if (nullptr == ServerSocket)
	{
		Log(FString::Printf(TEXT("StartTCPReceiver>> Listen socket could not be created! ~> %s %d"), *Ip, Port));
		return false;
	}

	//Set Buffer Size
	int32 NewSize = 0;
	ServerSocket->SetReceiveBufferSize((2 * 1024 * 1024), NewSize);

	//Start the Listener! //thread this eventually
	if (IsValid(World))
	{
		FTimerManager& WorldTimerManager = World->GetTimerManager();
		WorldTimerManager.SetTimer(MainProcTimerHandle, this, &UServerSocket::MainProc, 0.01f, true);
	}

	return true;
}

void UServerSocket::CloseSocket()
{
	if (nullptr == ServerSocket)
		return;

	ServerSocket->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ServerSocket);
	ServerSocket = nullptr;
}

void UServerSocket::BroadcastToClients(FString& SendData)
{
	TCHAR *serializedChar = SendData.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);

	for (FSocket* Socket : ConnectedClientsSocket)
	{
		if (Socket->GetConnectionState() != SCS_Connected)
			continue;

		int32 sent = 0;
		bool successful = Socket->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);
		if (false == successful)
		{
			TSharedRef<FInternetAddr> PeerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
			Socket->GetPeerAddress(*PeerAddr);
			Log(FString::Printf(TEXT("Send Data To Client Fail!! : IP : %s, Data : %s"), *PeerAddr->ToString(true), *SendData));
			continue;
		}

		TSharedRef<FInternetAddr> PeerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		Socket->GetPeerAddress(*PeerAddr);
		Log(FString::Printf(TEXT("Send Data To Client Success!!! : IP : %s, Data : %s"), *PeerAddr->ToString(true), *SendData));
	}
}

void UServerSocket::SendDataToClient(FSocket* TargetClient, FString& SendData)
{
	int32 index = 0;
	if (ConnectedClientsSocket.Find(TargetClient, index))
	{
		SendData.Append(TEXT("@DataEOF@"));

		if (TargetClient->GetConnectionState() == SCS_Connected)
		{
			TCHAR *serializedChar = SendData.GetCharArray().GetData();
			int32 size = FCString::Strlen(serializedChar);
			int32 sent = 0;
			bool successful = TargetClient->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);
			if (false == successful)
			{
				TSharedRef<FInternetAddr> PeerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
				TargetClient->GetPeerAddress(*PeerAddr);
				Log(FString::Printf(TEXT("Send Data To Client Fail!! : IP : %s, Data : %s"), *PeerAddr->ToString(true), *SendData));
			}
		}
	}
}

void UServerSocket::CheckDisconnectedClients()
{
	// Check Disconnected Clients
	TArray<FSocket*> DissconnectedSocketList;
	for (FSocket* Socket : ConnectedClientsSocket)
	{
		if (Socket->GetConnectionState() != SCS_Connected)
		{
			TSharedRef<FInternetAddr> PeerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
			Socket->GetPeerAddress(*PeerAddr);
			Log(FString::Printf(TEXT("Client Disconnected : IP:%s"), *PeerAddr->ToString(true)));
			DissconnectedSocketList.Add(Socket);
		}
	}

	for (FSocket* Socket : DissconnectedSocketList)
	{
		ConnectedClientsSocket.Remove(Socket);
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
	}
}

void UServerSocket::CheckNewClientConnection()
{
	if (nullptr == ServerSocket)
		return;

	bool Pending;
	while (ServerSocket->HasPendingConnection(Pending) && Pending)
	{
		//Remote address
		TSharedRef<FInternetAddr> PeerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		FSocket* NewConnectClient = ServerSocket->Accept(*PeerAddr, TEXT("NewConnectClient"));
		Log(FString::Printf(TEXT("New Client Connect: IP:%s"), *PeerAddr->ToString(true)));

		ConnectedClientsSocket.Add(NewConnectClient);
	}
}

void UServerSocket::CheckRecvClientData()
{
	for (FSocket* Socket : ConnectedClientsSocket)
	{
		if (Socket->GetConnectionState() != SCS_Connected)
			continue;

		TArray<uint8> ReceivedData;
		uint32 ReceivedDataSize = 0;
		while (Socket->HasPendingData(ReceivedDataSize))
		{
			ReceivedData.SetNumZeroed(FMath::Min(ReceivedDataSize, 65507u));
			int32 Read = 0;
			Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

			Log(FString::Printf(TEXT("Data Read! %d"), ReceivedData.Num()));
		}

		if (ReceivedData.Num() <= 0)
		{
			continue;
		}

		Log(FString::Printf(TEXT("Data Bytes Read ~> %d"), ReceivedData.Num()));

		FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
		Log(FString::Printf(TEXT("As String Data ~> %s"), *ReceivedUE4String));

		TArray<FString> JsonStringArr;
		ReceivedUE4String.ParseIntoArray(JsonStringArr, TEXT("@DataEOF@"));
		for (int i=0 ; i<JsonStringArr.Num() ; i++)
		{
			if (OnRecvDataFromClient.IsBound())
			{
				OnRecvDataFromClient.Broadcast(JsonStringArr[i]);
			}
		}
	}
}

void UServerSocket::MainProc()
{
	if (nullptr == ServerSocket)
		return;

	CheckDisconnectedClients();

	CheckNewClientConnection();

	CheckRecvClientData();
}

bool UServerSocket::FormatIP4ToNumber(const FString& Ip, uint8(&Out)[4])
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

FString UServerSocket::StringFromBinaryArray(TArray<uint8>& BinaryArray)
{
	BinaryArray.Add(0);
	// Add 0 termination. Even if the string is already 0-terminated, it doesn't change the results.
	// Create a string from a byte array. The string is expected to be 0 terminated (i.e. a byte set to 0).
	// Use UTF8_TO_TCHAR if needed.
	// If you happen to know the data is UTF-16 (USC2) formatted, you do not need any conversion to begin with.
	// Otherwise you might have to write your own conversion algorithm to convert between multilingual UTF-16 planes.
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}

void UServerSocket::Log(FString LogMessage)
{
	UE_LOG(LogServerSocket, Warning, TEXT("%s"), *LogMessage);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, LogMessage);
}