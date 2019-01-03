// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGM.h"
#include "Controller/LobbyPC.h"
#include "MultiplayNativeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "MultiplayNativeGI.h"
#include "ServerSocket.h"
#include "Paths.h"
#include "LobbyGS.h"
#include "StructureDefines.h"
#include "StructureDefines.h"
#include "GameFramework/PlayerController.h"
#include "JsonHelperStatics.h"
#include "FileManager.h"




ALobbyGM::ALobbyGM()
{
	PlayerControllerClass = ALobbyPC::StaticClass();
	GameStateClass = ALobbyGS::StaticClass();

	FSoftObjectPath ClassLoader(TEXT("Blueprint'/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter.ThirdPersonCharacter_C'"));
	UClass* PawnClass = Cast<UClass>(ClassLoader.TryLoad());
	if (IsValid(PawnClass))
	{
		DefaultPawnClass = PawnClass;
	}

	NextRoomPortIdx = 0;
	for (int i = 0 ; i < MAX_ROOM_COUNT ; i++)
	{
		PortEnable[i] = true;
	}

	PacketProcCallbackMap.Add(MAKE_PACKET_HEADER(EPacketMainType::Lobby, EPacketLobbySubType::CreateRoom), &ALobbyGM::OnLobby_CreateRoomComplete);
	PacketProcCallbackMap.Add(MAKE_PACKET_HEADER(EPacketMainType::Lobby, EPacketLobbySubType::RoomInfoChange), &ALobbyGM::OnLobby_RoomInfoChange);
	PacketProcCallbackMap.Add(MAKE_PACKET_HEADER(EPacketMainType::Lobby, EPacketLobbySubType::DestroyRoom), &ALobbyGM::OnLobby_DestroyRoom);
 	PacketProcCallbackMap.Add(MAKE_PACKET_HEADER(EPacketMainType::Battle, EPacketBattleSubType::UserLogout), &ALobbyGM::OnBattle_UserLogout);
}

void ALobbyGM::StartPlay()
{
	Super::StartPlay();

	//	FString IniFilePath = FPaths::LaunchDir();
	FString IniFilePath = FPaths::ProjectDir();
	IniFilePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*IniFilePath);
	IniFilePath.Append(TEXT("Test.ini"));

	FString IpString = UMultiplayNativeGI::ReadCustomIniPropertyString(*IniFilePath, TEXT("Serv Lobby Connection Info"), TEXT("IP"));
	FString PortString = UMultiplayNativeGI::ReadCustomIniPropertyString(*IniFilePath, TEXT("Serv Lobby Connection Info"), TEXT("PORT"));
	
	ServerSocket = NewObject<UServerSocket>();
	ServerSocket->CreateSocket(GetWorld(), TEXT("TestTCPSocket"), IpString, FCString::Atoi(*PortString));
	ServerSocket->OnRecvDataFromClient.AddDynamic(this, &ALobbyGM::OnRecvDataFromClient);
}

void ALobbyGM::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

// 	ALobbyGS* LobbyGS = GetGameState<ALobbyGS>();
// 	if (!IsValid(LobbyGS))
// 	{
// 		ErrorMessage = TEXT("LobbyGS is not valid!!!");
// 		return;
// 	}
// 
// 
// 	FString PlayerName = UGameplayStatics::ParseOption(Options, TEXT("PlayerName"));
// 	if (PlayerName.Len() == 0)
// 	{
// 		ErrorMessage = TEXT("이름 미입력.");
// 		return;
// 	}
// 
// 	if (LobbyGS->FindPlayer(PlayerName))
// 	{
// 		ErrorMessage = TEXT("해당 플레이어 이름은 이미 사용중입니다.");
// 		return;
// 	}
}

void ALobbyGM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PlayerControllerArr.Add(NewPlayer);
}

void ALobbyGM::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerController* ExitPlayerController = Cast<APlayerController>(Exiting);
	if (IsValid(ExitPlayerController))
	{
		PlayerControllerArr.Remove(ExitPlayerController);
	}

	ALobbyGS* LobbyGS = GetGameState<ALobbyGS>();
	if (IsValid(LobbyGS))
	{
		FRoomInfo RoomInfo;
		FString LogoutPlayerName = ExitPlayerController->PlayerState->GetPlayerName();
		if (LobbyGS->GetPlayerJoinedRoomInfo(LogoutPlayerName, RoomInfo))
		{
			RoomInfo.PlayerList.Remove(LogoutPlayerName);
			LobbyGS->ChangeRoomInfo(RoomInfo);
		}
	}
}

// void ALobbyGM::HandleSeamlessTravelPlayer(AController*& C)
// {
// 	Super::HandleSeamlessTravelPlayer(C);
// 
// 	APlayerController* NewPlayer = Cast<APlayerController>(C);
// 	if (IsValid(NewPlayer))
// 	{
// 		PlayerControllerArr.Add(NewPlayer);
// 		if (!IsValid(RoomManagerPC))
// 		{
// 			RoomManagerElect();
// 		}
// 	}
// }

// void ALobbyGM::RoomManagerElect()
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

void ALobbyGM::CreateBattleRoom(APlayerController* RequestPC)
{
	ALobbyGS* LobbyGS = GetGameState<ALobbyGS>();
	if (!IsValid(LobbyGS))
		return;

	ALobbyPC* LobbyPC = Cast<ALobbyPC>(RequestPC);
	if (!IsValid(LobbyPC))
		return;

	FString RequestPlayerName = RequestPC->PlayerState->GetPlayerName();
	if (LobbyGS->FindPlayer(RequestPlayerName))
	{
		return;
	}
	
	int EnablePortIndex = -1;
	for (int i=0 ; i<MAX_ROOM_COUNT ; i++)
	{
		if (true == PortEnable[i])
		{
			EnablePortIndex = i;
			break;
		}
	}

	if (-1 == EnablePortIndex)
		return;

	int PortNumber = EnablePortIndex + 8000;

	TCHAR FIlePath[MAX_PATH] = { 0, };
	GetModuleFileName(nullptr, FIlePath, MAX_PATH);
	
	FString	Params(TEXT(""));
	Params += TEXT("BattleWait ");
	Params += TEXT("-server ");
	Params += TEXT("-log ");
	Params += TEXT("-nosteam ");
	Params += FString::Printf(TEXT("-RoomNo=%d "), NextRoomNumber);
	Params += FString::Printf(TEXT("-Port=%d "), PortNumber);
	
	FString exePath = FPaths::GetCleanFilename(FIlePath);

	FPlatformProcess::CreateProc(FIlePath, *Params, true, false, false, nullptr, 0, nullptr, nullptr);
//	FPlatformProcess::CreateProc(FIlePath, TEXT("Battle -server -log -nosteam"), true, false, false, nullptr, 0, nullptr, nullptr);
	
	FRoomInfo NewBattleRoom;
	NewBattleRoom.RoomNo = NextRoomNumber;
	NewBattleRoom.Port = PortNumber;
	NewBattleRoom.BattleState = EBattleState::Createing;
	NewBattleRoom.PlayerList.Add(RequestPlayerName);
	LobbyGS->RoomList.Add(NewBattleRoom);

	PortEnable[EnablePortIndex] = false;

	LobbyPC->JoinedRoomNo = NewBattleRoom.RoomNo;

	NextRoomNumber++;
}

bool ALobbyGM::FIndPlayer(const FString& PlayerName)
{
	for (APlayerController* PlayerController : PlayerControllerArr)
	{
		if (PlayerController->PlayerState->GetPlayerName() == PlayerName)
		{
			return true;
		}
	}

	ALobbyGS* LobbyGS = GetGameState<ALobbyGS>();
	if (IsValid(LobbyGS))
	{
		return LobbyGS->FindPlayer(PlayerName);
	}

	return false;
}

void ALobbyGM::OnRecvDataFromClient(const FString& DataString)
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

void ALobbyGM::OnLobby_CreateRoomComplete(const TSharedPtr<FJsonObject>& JsonObject)
{
	FPacketRoomCreateComplete PacketRoomCreateComplete;
	if (PacketRoomCreateComplete.ParseJsonObject(JsonObject->GetObjectField(PacketRoomCreateComplete.GetJsonHeaderString())) == false)
		return;

	ALobbyGS* LobbyGS = GetGameState<ALobbyGS>();
	if (!IsValid(LobbyGS))
		return;

	FRoomInfo RoomInfo;
	if (!LobbyGS->GetRoomInfo(PacketRoomCreateComplete.RoomNo, RoomInfo))
		return;

	for (const FString& PlayerName : RoomInfo.PlayerList)
	{
		for (APlayerController* PC : PlayerControllerArr)
		{
			ALobbyPC* LobbyPc = Cast<ALobbyPC>(PC);
			if (!IsValid(LobbyPc))
				continue;

			if (PlayerName == LobbyPc->PlayerState->GetPlayerName())
				LobbyPc->Clnt_WarpRoom(RoomInfo.Port);
		}
	}

	RoomInfo.BattleState = EBattleState::WaitingPlayers;
	LobbyGS->ChangeRoomInfo(RoomInfo);
}

void ALobbyGM::OnLobby_RoomInfoChange(const TSharedPtr<FJsonObject>& JsonObject)
{
	ALobbyGS* LobbyGS = GetGameState<ALobbyGS>();
	if (!IsValid(LobbyGS))
		return;

	FPacketChageRoomInfo PacketChageRoomInfo;
	if (PacketChageRoomInfo.ParseJsonObject(JsonObject->GetObjectField(PacketChageRoomInfo.GetJsonHeaderString())) == false)
		return;

	FRoomInfo NewRoomInfo;
	if (LobbyGS->GetRoomInfo(PacketChageRoomInfo.RoomNo, NewRoomInfo))
	{
		NewRoomInfo.PlayerList = PacketChageRoomInfo.PlayerList;
		NewRoomInfo.BattleState = static_cast<EBattleState>(PacketChageRoomInfo.RoomState);

		LobbyGS->ChangeRoomInfo(NewRoomInfo);
	}	
}

void ALobbyGM::OnLobby_DestroyRoom(const TSharedPtr<FJsonObject>& JsonObject)
{
	ALobbyGS* LobbyGS = GetGameState<ALobbyGS>();
	if (!IsValid(LobbyGS))
		return;

	FPacketDeleteRoom PacketDeleteRoom;
	if (PacketDeleteRoom.ParseJsonObject(JsonObject->GetObjectField(PacketDeleteRoom.GetJsonHeaderString())) == false)
		return;

	LobbyGS->DeleteRoom(PacketDeleteRoom.RoomNo);
}

void ALobbyGM::OnBattle_UserLogout(const TSharedPtr<FJsonObject>& JsonObject)
{
	ALobbyGS* LobbyGS = GetGameState<ALobbyGS>();
	if (!IsValid(LobbyGS))
		return;

	FPacketBattleUserLogout PacketBattleUserLogout;
	if (PacketBattleUserLogout.ParseJsonObject(JsonObject->GetObjectField(PacketBattleUserLogout.GetJsonHeaderString())) == false)
		return;

	FRoomInfo RoomInfo;
	if (LobbyGS->GetRoomInfo(PacketBattleUserLogout.RoomNo, RoomInfo))
	{
		RoomInfo.PlayerList.Remove(PacketBattleUserLogout.LogoutUserName);
	}
}