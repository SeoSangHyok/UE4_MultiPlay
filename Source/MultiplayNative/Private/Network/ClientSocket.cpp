// Fill out your copyright notice in the Description page of Project Settings.

#include "ClientSocket.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "../Networking/Public/Interfaces/IPv4/IPv4Endpoint.h"
#include "../Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "../Networking/Public/Common/TcpSocketBuilder.h"
#include "TimerManager.h"
#include "SocketSubsystem.h"

DEFINE_LOG_CATEGORY(LogClientSocket);


bool UClientSocket::ConnectToServer(UWorld* World, const FString& SocketName, const FString& Ip, const int32 Port)
{
	if (nullptr != ClientSocket)
	{
		Log(FString::Printf(TEXT("Socket Already Created. Try After CloseSocket().")));
		return false;
	}

	uint8 Ip4Nums[4] = { 0, };
	if (!FormatIP4ToNumber(Ip, Ip4Nums))
	{
		Log(FString::Printf(TEXT("Invalid IP! Expecting 4 parts separated by . IP : %s"), *Ip));
		return false;
	}

	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (nullptr == SocketSubsystem)
	{
		Log(FString::Printf(TEXT("Gettting SocketSubsystem Fail!!")));
		return false;
	}

	ClientSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
	if (nullptr == ClientSocket)
	{
		Log(FString::Printf(TEXT("ClientSocket Create Fail!!")));
		return false;
	}

	FIPv4Address ip(Ip4Nums[0], Ip4Nums[1], Ip4Nums[2], Ip4Nums[3]);
	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(Port);

	if (ClientSocket->Connect(*addr) == false)
	{
		Log(FString::Printf(TEXT("ClientSocket Connect Fail!! : IP : %s"), *addr->ToString(true)));
		CloseSocket();
		return false;
	}

	//Start the Listener! //thread this eventually
	if (!IsValid(World))
	{
		Log(FString::Printf(TEXT("World is Not valid!! socket proc is not working!!!")));
		return false;
	}

	FTimerManager& WorldTimerManager = World->GetTimerManager();
	WorldTimerManager.SetTimer(MainProcTimerHandle, this, &UClientSocket::MainProc, 0.01f, true);

	return true;
}

void UClientSocket::MainProc()
{
	TArray<uint8> ReceivedData;
	uint32 ReceivedDataSize = 0;
	while (ClientSocket->HasPendingData(ReceivedDataSize))
	{
		ReceivedData.SetNumZeroed(FMath::Min(ReceivedDataSize, 65507u));
		int32 Read = 0;
		ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

		Log(FString::Printf(TEXT("Data Read! %d"), ReceivedData.Num()));
	}

	if (ReceivedData.Num() <= 0)
	{
		return;
	}

	Log(FString::Printf(TEXT("Data Bytes Read ~> %d"), ReceivedData.Num()));

	const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
	Log(FString::Printf(TEXT("As String Data ~> %s"), *ReceivedUE4String));

	TArray<FString> JsonStringArr;
	ReceivedUE4String.ParseIntoArray(JsonStringArr, TEXT("@DataEOF@"));
	for (int i = 0; i < JsonStringArr.Num(); i++)
	{
		if (OnRecvDataFromServer.IsBound())
		{
			OnRecvDataFromServer.Broadcast(ReceivedUE4String);
		}
	}
}

void UClientSocket::CloseSocket()
{
	ClientSocket->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
	ClientSocket = nullptr;
}

void UClientSocket::SendDataToServer(FString& SendData)
{
	if (nullptr == ClientSocket)
		return;

	if (ClientSocket->GetConnectionState() != SCS_Connected)
	{
		Log(FString::Printf(TEXT("UClientSocket::SendDataToServer ==> ClientSocket is Not Connected!!")));
		return;
	}

	SendData.Append(TEXT("@DataEOF@"));
	TCHAR *serializedChar = SendData.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);
	int32 sent = 0;
	bool successful = ClientSocket->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);
	if (false == successful)
	{
		TSharedRef<FInternetAddr> PeerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		ClientSocket->GetPeerAddress(*PeerAddr);
		Log(FString::Printf(TEXT("Send Data To Client Fail!! : IP : %s, Data : %s"), *PeerAddr->ToString(true), *SendData));
	}
}

bool UClientSocket::FormatIP4ToNumber(const FString& Ip, uint8(&Out)[4])
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

FString UClientSocket::StringFromBinaryArray(TArray<uint8>& BinaryArray)
{
	BinaryArray.Add(0);
	// Add 0 termination. Even if the string is already 0-terminated, it doesn't change the results.
	// Create a string from a byte array. The string is expected to be 0 terminated (i.e. a byte set to 0).
	// Use UTF8_TO_TCHAR if needed.
	// If you happen to know the data is UTF-16 (USC2) formatted, you do not need any conversion to begin with.
	// Otherwise you might have to write your own conversion algorithm to convert between multilingual UTF-16 planes.
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}

void UClientSocket::Log(FString LogMessage)
{
	UE_LOG(LogClientSocket, Warning, TEXT("%s"), *LogMessage);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, LogMessage);
}