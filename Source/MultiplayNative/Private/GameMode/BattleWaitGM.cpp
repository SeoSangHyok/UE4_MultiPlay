// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleWaitGM.h"
#include "Controller/LobbyPC.h"
#include "MultiplayNativeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "MultiplayNativeGI.h"
#include "ServerSocket.h"
#include "Paths.h"
#include "StructureDefines.h"
#include "StructureDefines.h"
#include "GameFramework/PlayerController.h"
#include "PacketProcessor.h"
#include "JsonHelperStatics.h"
#include "BattleWaitGS.h"



ABattleWaitGM::ABattleWaitGM()
{
	PlayerControllerClass = ALobbyPC::StaticClass();
	GameStateClass = ABattleWaitGS::StaticClass();

	FSoftObjectPath ClassLoader(TEXT("Blueprint'/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter.ThirdPersonCharacter_C'"));
	UClass* PawnClass = Cast<UClass>(ClassLoader.TryLoad());
	if (IsValid(PawnClass))
	{
		DefaultPawnClass = PawnClass;
	}

	NextRoomPortIdx = 0;
	for (int i = 0; i < 100; i++)
	{
		PortEnable[i] = true;
	}

	PacketProcCallbackMap.Add(MAKE_PACKET_HEADER(EPacketMainType::Lobby, EPacketLobbySubType::CreateRoom), &ABattleWaitGM::OnLobby_CreateRoomComplete);
}

void ABattleWaitGM::StartPlay()
{
	Super::StartPlay();

// 	ServerSocket = NewObject<UServerSocket>();
// 	ServerSocket->CreateSocket(GetWorld(), TEXT("TestTCPSocket"), TEXT("127.0.0.1"), 7777);
// 
// 	ServerSocket->OnRecvDataFromClient.AddDynamic(this, &ABattleWaitGM::OnRecvDataFromClient);
// 
// 	FPacketHeaderType PacketHeaderType = MAKE_PACKET_HEADER(EPacketMainType::Lobby, EPacketLobbySubType::CreateRoom);
// 
// 	FPacketRoomCreateComplete PacketRoomCreateComplete;
// 	PacketRoomCreateComplete.RoomNo = RoomNo;
// 
// 	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
// 	JsonObj->SetObjectField(PacketHeaderType.GetJsonHeaderString(), PacketHeaderType.MakeJsonObject());
// 	JsonObj->SetObjectField(PacketRoomCreateComplete.GetJsonHeaderString(), PacketRoomCreateComplete.MakeJsonObject());
// 
// 	FString DataString = UJsonHelperStatics::MakeJsonString(JsonObj);
// 	SocketToMasterServ->SendDataToServer(DataString);
}

void ABattleWaitGM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PlayerControllerArr.Add(NewPlayer);
	// 	if (!IsValid(RoomManagerPC))
	// 	{
	// 		RoomManagerElect();
	// 	}
}

void ABattleWaitGM::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerController* ExitPlayerController = Cast<APlayerController>(Exiting);
	if (IsValid(ExitPlayerController))
	{
		PlayerControllerArr.Remove(ExitPlayerController);
		// 		if (RoomManagerPC == ExitPlayerController)
		// 		{
		// 			RoomManagerElect();
		// 		}
	}
}

void ABattleWaitGM::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	APlayerController* NewPlayer = Cast<APlayerController>(C);
	if (IsValid(NewPlayer))
	{
		PlayerControllerArr.Add(NewPlayer);
		if (!IsValid(RoomManagerPC))
		{
//			RoomManagerElect();
		}
	}
}

// void ABattleWaitGM::RoomManagerElect()
// {
// 	if (PlayerControllerArr.Num() == 0)
// 	{
// 		RoomManagerPC = nullptr;
// 		return;
// 	}
// 
// 	int NewRoomManagerIdx = FMath::Rand() % PlayerControllerArr.Num();
// 	RoomManagerPC = PlayerControllerArr[NewRoomManagerIdx];
// 
// 	ALobbyPC* RoomManagerLobbyPc = Cast<ALobbyPC>(RoomManagerPC);
// 	if (IsValid(RoomManagerLobbyPc))
// 	{
// 		RoomManagerLobbyPc->IsRoomManager = true;
// 		RoomManagerLobbyPc->Client_RoomManagerElected();
// 	}
// }

// void ABattleWaitGM::CreateBattleRoom(APlayerController* RequestPC)
// {
// 	ABattleWaitGS* LobbyGS = GetGameState<ABattleWaitGS>();
// 	if (!IsValid(LobbyGS))
// 		return;
// 
// 	FString RequestPlayerName = RequestPC->PlayerState->GetPlayerName();
// 	for (const FRoomInfo& RoomInfo : LobbyGS->RoomList)
// 	{
// 		for (const FString& PlayerName : RoomInfo.PlayerList)
// 		{
// 			if (PlayerName == RequestPlayerName)
// 			{
// 				return;
// 			}
// 		}
// 	}
// 
// 	int EnablePortIndex = -1;
// 	for (int i = 0; i<100; i++)
// 	{
// 		if (true == PortEnable[i])
// 		{
// 			EnablePortIndex = i;
// 			break;
// 		}
// 	}
// 
// 	if (-1 == EnablePortIndex)
// 		return;
// 
// 	int PortNumber = EnablePortIndex + 8000;
// 
// 	TCHAR FIlePath[MAX_PATH] = { 0, };
// 	GetModuleFileName(nullptr, FIlePath, MAX_PATH);
// 
// 	FString	Params(TEXT(""));
// 	Params += TEXT("Battle ");
// 	Params += TEXT("-server ");
// 	Params += TEXT("-log ");
// 	Params += TEXT("-nosteam ");
// 	Params += FString::Printf(TEXT("-RoomNo=%d "), NextRoomNumber);
// 	Params += FString::Printf(TEXT("-Port=%d "), PortNumber);
// 
// 	FString exePath = FPaths::GetCleanFilename(FIlePath);
// 
// 	FPlatformProcess::CreateProc(FIlePath, *Params, true, false, false, nullptr, 0, nullptr, nullptr);
// 	//	FPlatformProcess::CreateProc(FIlePath, TEXT("Battle -server -log -nosteam"), true, false, false, nullptr, 0, nullptr, nullptr);
// 
// 	FRoomInfo NewBattleRoom;
// 	NewBattleRoom.RoomNo = NextRoomNumber;
// 	NewBattleRoom.Port = PortNumber;
// 	NewBattleRoom.BattleState = EBattleState::WaitingPlayers;
// 	NewBattleRoom.PlayerList.Add(RequestPlayerName);
// 	LobbyGS->RoomList.Add(NewBattleRoom);
// 
// 	PortEnable[EnablePortIndex] = false;
// 	NextRoomNumber++;
// }

void ABattleWaitGM::OnRecvDataFromClient(const FString& DataString)
{
	TSharedPtr<FJsonObject>JsonObject = UJsonHelperStatics::ParseJsonString(DataString);

	FPacketHeaderType PacketHeaderType;
	PacketHeaderType.ParseJsonObject(JsonObject->GetObjectField(PacketHeaderType.GetJsonHeaderString()));

	if (PacketProcCallbackMap.Find(PacketHeaderType))
	{
		PacketProcCallback& Callback = PacketProcCallbackMap[PacketHeaderType];
		(this->*(Callback))(JsonObject);
	}
}

void ABattleWaitGM::OnLobby_CreateRoomComplete(const TSharedPtr<FJsonObject>& JsonObject)
{
	FPacketRoomCreateComplete PacketRoomCreateComplete;
	if (PacketRoomCreateComplete.ParseJsonObject(JsonObject->GetObjectField(PacketRoomCreateComplete.GetJsonHeaderString())) == false)
		return;

	ABattleWaitGS* LobbyGS = GetGameState<ABattleWaitGS>();
	if (!IsValid(LobbyGS))
		return;

	for (const FRoomInfo& RoomInfo : LobbyGS->RoomList)
	{
		if (RoomInfo.RoomNo == PacketRoomCreateComplete.RoomNo)
		{
			for (const FString& PlayerName : RoomInfo.PlayerList)
			{
				for (APlayerController* PC : PlayerControllerArr)
				{
					if (PlayerName == PC->PlayerState->GetPlayerName())
					{
						ALobbyPC* LobbyPc = Cast<ALobbyPC>(PC);
						if (IsValid(PC))
						{
							LobbyPc->Clnt_WarpRoom(RoomInfo.Port);
						}
					}
				}
			}

			break;
		}
	}
}