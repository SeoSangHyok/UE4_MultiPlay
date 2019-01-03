// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleGM.h"
#include "BattlePC.h"
#include "Kismet/GameplayStatics.h"
#include "LobbyWidget_WS.h"
#include "TimerManager.h"
#include "EnumDefines.h"
#include "BattleGS.h"
#include "BattlePS.h"
#include "GameFramework/PlayerStart.h"
#include "StructureDefines.h"
#include "ClientSocket.h"
#include "PacketDefine.h"
#include "JsonHelperStatics.h"
#include "Engine/Engine.h"
#include "Paths.h"
#include "FileManager.h"
#include "MultiplayNativeGI.h"


ABattleGM::ABattleGM()
{
	PlayerControllerClass = ABattlePC::StaticClass();
	GameStateClass = ABattleGS::StaticClass();
	PlayerStateClass = ABattlePS::StaticClass();

	FSoftObjectPath ClassLoader(TEXT("Blueprint'/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter.ThirdPersonCharacter_C'"));
	UClass* PawnClass = Cast<UClass>(ClassLoader.TryLoad());
	if (IsValid(PawnClass))
	{
		DefaultPawnClass = PawnClass;
	}

	BattleState = EBattleState::Createing;
	bUseSeamlessTravel = true;
}

ABattleGM::~ABattleGM()
{
// 	if (IsValid(SocketToMasterServ))
// 	{
// 		SocketToMasterServ->CloseSocket();
// 	}

// 	FPacketHeaderType PacketHeaderType = MAKE_PACKET_HEADER(EPacketMainType::Lobby, EPacketLobbySubType::DestroyRoom);
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

void ABattleGM::StartPlay()
{
	Super::StartPlay();

	BattleGS = GetGameState<ABattleGS>();

	GetWorldTimerManager().SetTimer(MainTimerHandle, this, &ABattleGM::MainTImer, 1.0f, true);

	RemainPlayingTime = 60;
	BattleGS->MainMessage = TEXT("다른플레이어 입장을 기다리고 있습니다.");


	//	FString IniFilePath = FPaths::LaunchDir();
	FString IniFilePath = FPaths::ProjectDir();
	IniFilePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*IniFilePath);
	IniFilePath.Append(TEXT("Test.ini"));

	FString IpString = UMultiplayNativeGI::ReadCustomIniPropertyString(*IniFilePath, TEXT("Serv Lobby Connection Info"), TEXT("IP"));
	FString PortString = UMultiplayNativeGI::ReadCustomIniPropertyString(*IniFilePath, TEXT("Serv Lobby Connection Info"), TEXT("PORT"));

	SocketToMasterServ = NewObject<UClientSocket>();
	SocketToMasterServ->ConnectToServer(GetWorld(), TEXT("SocketToMasterServ"), IpString, FCString::Atoi(*PortString));
	SocketToMasterServ->OnRecvDataFromServer.AddDynamic(this, &ABattleGM::OnRecvDataFromServer);

	bool IsServerTravel = UGameplayStatics::GetIntOption(OptionsString, TEXT("IsServerTravel"), 0) == 0 ? false : true;
	if (IsServerTravel)
	{
		PrevLevelNumPlayers = UGameplayStatics::GetIntOption(OptionsString, TEXT("PrevLevelNumPlayers"), 0);
		RoomNo = UGameplayStatics::GetIntOption(OptionsString, TEXT("RoomNo"), 0);

		BattleGS->RoomNo = RoomNo;
		BattleState = EBattleState::WaitingPlay;
		BattleGS->BattleState = BattleState;
	}
	else
	{
		TArray<FString> Tokens;
		TArray<FString> Switches;
		FCommandLine::Parse(FCommandLine::Get(), Tokens, Switches);

		SocketToMasterServ->Log(FCommandLine::Get());

		for (FString& Token : Tokens)
		{
			if (Token.Contains(TEXT("RoomNo")))
			{
				Token.RemoveSpacesInline();
				int32 delimIdx;
				if (Token.FindChar('=', delimIdx))
				{
					FString RoomNoString = Token.Mid((delimIdx + 1), Token.Len() - (delimIdx + 1));
					SocketToMasterServ->Log(RoomNoString);
					RoomNo = FCString::Atoi(*RoomNoString);
				}
				break;
			}
		}

		BattleGS->RoomNo = RoomNo;
		BattleState = EBattleState::WaitingPlayers;
		BattleGS->BattleState = BattleState;

		FPacketHeaderType PacketHeaderType = MAKE_PACKET_HEADER(EPacketMainType::Lobby, EPacketLobbySubType::CreateRoom);

		FPacketRoomCreateComplete PacketRoomCreateComplete;
		PacketRoomCreateComplete.RoomNo = RoomNo;

		TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
		JsonObj->SetObjectField(PacketHeaderType.GetJsonHeaderString(), PacketHeaderType.MakeJsonObject());
		JsonObj->SetObjectField(PacketRoomCreateComplete.GetJsonHeaderString(), PacketRoomCreateComplete.MakeJsonObject());

		FString DataString = UJsonHelperStatics::MakeJsonString(JsonObj);
		if (SocketToMasterServ)
		{
			SocketToMasterServ->SendDataToServer(DataString);
		}		
	}
}

void ABattleGM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ABattlePC* BattlePC = Cast<ABattlePC>(NewPlayer);
	BattlePCs.Add(BattlePC);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ABattleGM::PostLogin"));

	ABattlePS* BattlePS = Cast<ABattlePS>(NewPlayer->PlayerState);
	for (auto TargetBattlePC : BattlePCs)
	{
		TargetBattlePC->Clnt_PlayerLogin(BattlePS->GetPlayerName());
	}

	if (!IsValid(RoomManagerPC))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RoomManagerElect() start"));
		RoomManagerElect();
	}
}

void ABattleGM::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ABattlePS* BattlePS = Cast<ABattlePS>(Exiting->PlayerState);
	for (auto TargetBattlePC : BattlePCs)
	{
		TargetBattlePC->Clnt_PlayerLogin(BattlePS->GetPlayerName());
	}

	ABattlePC* BattlePC = Cast<ABattlePC>(Exiting);
	BattlePCs.Remove(BattlePC);

	BattleGS->PlayerList.Remove(BattlePC->PlayerState->GetPlayerName());

	if (BattlePC == RoomManagerPC)
	{
		RoomManagerElect();
	}

	FPacketHeaderType PacketHeaderType = MAKE_PACKET_HEADER(EPacketMainType::Lobby, EPacketLobbySubType::RoomInfoChange);

	FPacketChageRoomInfo PacketChageRoomInfo;
	PacketChageRoomInfo.RoomNo = BattleGS->RoomNo;
	PacketChageRoomInfo.RoomState = static_cast<int>(BattleState);
	PacketChageRoomInfo.PlayerList = BattleGS->PlayerList;

	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	JsonObj->SetObjectField(PacketHeaderType.GetJsonHeaderString(), PacketHeaderType.MakeJsonObject());
	JsonObj->SetObjectField(PacketChageRoomInfo.GetJsonHeaderString(), PacketChageRoomInfo.MakeJsonObject());

	FString DataString = UJsonHelperStatics::MakeJsonString(JsonObj);
	if (SocketToMasterServ)
	{
		SocketToMasterServ->SendDataToServer(DataString);
	}	

	if (BattlePCs.Num() == 0)
	{
#if WITH_EDITOR
#else
		GIsRequestingExit = true;
#endif
		FPacketHeaderType PacketHeaderType = MAKE_PACKET_HEADER(EPacketMainType::Lobby, EPacketLobbySubType::DestroyRoom);
		FPacketDeleteRoom PacketDeleteRoom;
		PacketDeleteRoom.RoomNo = BattleGS->RoomNo;

		TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
		JsonObj->SetObjectField(PacketHeaderType.GetJsonHeaderString(), PacketHeaderType.MakeJsonObject());
		JsonObj->SetObjectField(PacketDeleteRoom.GetJsonHeaderString(), PacketDeleteRoom.MakeJsonObject());

		FString DataString = UJsonHelperStatics::MakeJsonString(JsonObj);
		if (SocketToMasterServ)
		{
			SocketToMasterServ->SendDataToServer(DataString);
		}		

//		FGenericPlatformMisc::RequestExit(true);
//		UKismetSystemLibrary::QuitGame(GetOwningPlayer()->GetWorld(), GetOwningPlayer(), EQuitPreference::Type::Quit);
	}
}

void ABattleGM::RestartPlayer(AController* NewPlayer)
{
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	if (PlayerStartList.Num() == 0)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartList);
	}


	int PlayerStartIndex = FMath::Rand() % PlayerStartList.Num();
	AActor* StartSpot = PlayerStartList[PlayerStartIndex];

	// If a start spot wasn't found,
	if (StartSpot == nullptr)
	{
		// Check for a previously assigned spot
		if (NewPlayer->StartSpot != nullptr)
		{
			StartSpot = NewPlayer->StartSpot.Get();
			UE_LOG(LogGameMode, Warning, TEXT("RestartPlayer: Player start not found, using last start spot"));
		}
	}

	RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}

void ABattleGM::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	ABattlePC* BattlePC = Cast<ABattlePC>(C);
	BattlePCs.Add(BattlePC);

	ABattlePS* BattlePS = Cast<ABattlePS>(BattlePC->PlayerState);
	for (auto TargetBattlePC : BattlePCs)
	{
		TargetBattlePC->Clnt_PlayerLogin(BattlePS->GetPlayerName());
	}
}

void ABattleGM::MainTImer()
{
	switch (BattleState)
	{
	case EBattleState::WaitingPlay:
		if (CheckPlayerJoinComplete())
		{
			BattleState = EBattleState::Playing;
			BattleGS->MainMessage = TEXT("게임이 시작되었습니다.");
		}
		else
		{
			if (UGameplayStatics::GetRealTimeSeconds(GetWorld()) >= 30.0f)
			{
				BattleState = EBattleState::Playing;
				BattleGS->MainMessage = TEXT("게임이 시작되었습니다.");
			}
		}
		break;

	case EBattleState::Playing:
		RemainPlayingTime--;
		if (0>=RemainPlayingTime)
		{
			FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
			if (LevelName == TEXT("Battle"))
			{
				int PlayerNum = GetNumPlayers();
				for (int i = 0; i<PlayerNum; i++)
				{
					ABattlePC* TargetPC = Cast<ABattlePC>(UGameplayStatics::GetPlayerController(GetWorld(), i));
					if (IsValid(TargetPC))
					{
						TargetPC->Clnt_ReadyToStartGame();
					}
				}

				UWorld* World = GetWorld();
				if (IsValid(World))
				{
					FString URL = TEXT("Battle2");
					URL = URL + TEXT("?PrevLevelNumPlayers=") + FString::FromInt(PlayerNum);
					URL = URL + TEXT("?IsServerTravel=1");
					URL = URL + TEXT("?RoomNo=") + FString::FromInt(RoomNo);
					World->ServerTravel(URL);
				}
			}
			else
			{
				BattleState = EBattleState::Finish;

				FTimerDelegate TimerCallback;
				TimerCallback.BindLambda([&]
				{
					int PlayerNum = GetNumPlayers();
					for (int i = 0; i<PlayerNum; i++)
					{
						ABattlePC* TargetPC = Cast<ABattlePC>(UGameplayStatics::GetPlayerController(GetWorld(), i));
						if (IsValid(TargetPC))
						{
							TargetPC->Clnt_ReadyToStartGame();
							TargetPC->Clnt_GotoLobby();
						}
					}

// 					UWorld* World = GetWorld();
// 					if (IsValid(World))
// 					{
// 						FString URL = TEXT("Lobby");
// 						World->ServerTravel(URL);
// 					}
				});

				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(TimerHandle, TimerCallback, 10.0f, false);

				BattleGS->MainMessage = FString::Printf(TEXT("게임이 끝났습니다 10 초후에 로비로 이동합니다."));


				CalcGameResult();
			}


			GetWorldTimerManager().ClearTimer(MainTimerHandle);
		}
		else
		{
			BattleGS->MainMessage = FString::Printf(TEXT("남은 게임시간 : %d"), RemainPlayingTime);
		}
		break;

	case EBattleState::Finish:
		break;
	}
}

void ABattleGM::OnRecvDataFromServer(const FString& DataString)
{

}

void ABattleGM::AddPlayer(FString PlayerName)
{
	ABattleGS* BattleGS = Cast<ABattleGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (IsValid(BattleGS))
	{
		if (BattleGS->PlayerList.Find(PlayerName) == INDEX_NONE)
		{
			BattleGS->PlayerList.Add(PlayerName);

			FPacketHeaderType PacketHeaderType = MAKE_PACKET_HEADER(EPacketMainType::Lobby, EPacketLobbySubType::RoomInfoChange);

			FPacketChageRoomInfo PacketChageRoomInfo;
			PacketChageRoomInfo.RoomNo = BattleGS->RoomNo;
			PacketChageRoomInfo.RoomState = static_cast<int>(BattleState);
			PacketChageRoomInfo.PlayerList = BattleGS->PlayerList;

			TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
			JsonObj->SetObjectField(PacketHeaderType.GetJsonHeaderString(), PacketHeaderType.MakeJsonObject());
			JsonObj->SetObjectField(PacketChageRoomInfo.GetJsonHeaderString(), PacketChageRoomInfo.MakeJsonObject());

			FString DataString = UJsonHelperStatics::MakeJsonString(JsonObj);
			if (nullptr != SocketToMasterServ)
			{
				SocketToMasterServ->SendDataToServer(DataString);
			}
		}
	}
}

void ABattleGM::RoomManagerElect()
{
	if (BattlePCs.Num() == 0)
		return;

	int NewRoomMangerIndex = FMath::Rand() % BattlePCs.Num();
	RoomManagerPC = BattlePCs[NewRoomMangerIndex];
	RoomManagerPC->Clnt_RoomManagerElected();
}

void ABattleGM::StartBattle()
{
	if (BattleGS->BattleState != EBattleState::WaitingPlayers)
		return;

	BattleState = EBattleState::Playing;
	BattleGS->BattleState = EBattleState::Playing;

	for (int i = 0; i<BattlePCs.Num(); i++)
	{
		if (IsValid(BattlePCs[i]))
		{
			BattlePCs[i]->Clnt_ReadyToStartGame();
		}
	}

	//UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Battle")));
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		FString URL = TEXT("Battle");
		URL = URL + TEXT("?PrevLevelNumPlayers=") + FString::FromInt(BattlePCs.Num());
		URL = URL + TEXT("?IsServerTravel=1");
		URL = URL + TEXT("?RoomNo=") + FString::FromInt(RoomNo);
		World->ServerTravel(URL);
		//			World->ServerTravel(FString(TEXT("Battle")));
	}

	FPacketHeaderType PacketHeaderType = MAKE_PACKET_HEADER(EPacketMainType::Lobby, EPacketLobbySubType::RoomInfoChange);

	FPacketChageRoomInfo PacketChageRoomInfo;
	PacketChageRoomInfo.RoomNo = BattleGS->RoomNo;
	PacketChageRoomInfo.RoomState = static_cast<int>(BattleState);
	PacketChageRoomInfo.PlayerList = BattleGS->PlayerList;

	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	JsonObj->SetObjectField(PacketHeaderType.GetJsonHeaderString(), PacketHeaderType.MakeJsonObject());
	JsonObj->SetObjectField(PacketChageRoomInfo.GetJsonHeaderString(), PacketChageRoomInfo.MakeJsonObject());

	FString DataString = UJsonHelperStatics::MakeJsonString(JsonObj);
	if (nullptr != SocketToMasterServ)
	{
		SocketToMasterServ->SendDataToServer(DataString);
	}
}

bool ABattleGM::CheckPlayerJoinComplete()
{
	return (BattlePCs.Num() == PrevLevelNumPlayers);
}

void ABattleGM::CalcGameResult()
{
	if (!HasAuthority())
		return;

	for (ABattlePC* BattlePC : BattlePCs)
	{
		if (IsValid(BattlePC))
		{
			ABattlePS* BattlePS = Cast<ABattlePS>(BattlePC->PlayerState);
			if (IsValid(BattlePS))
			{
				int PlayerScore = (BattlePS->KillCount * 3) + (BattlePS->DeathCount*-1);

				FPlayerResultItem PlayerResultItem;
				PlayerResultItem.PlayerName = BattlePS->GetPlayerName();
				PlayerResultItem.Score = PlayerScore;
				PlayerResultItem.Kill = BattlePS->KillCount;
				PlayerResultItem.Death = BattlePS->DeathCount;
				PlayerResults.Add(PlayerResultItem);
			}
		}		
	}

	PlayerResults.Sort([](const FPlayerResultItem& A, const FPlayerResultItem& B) {
		return A.Score > B.Score;
	});

	int Rank = 1;
	for (FPlayerResultItem& PlayerResultItem : PlayerResults)
	{
		PlayerResultItem.Rank = Rank;
		Rank++;
	}

	for (ABattlePC* BattlePC : BattlePCs)
	{
		if (IsValid(BattlePC))
		{
			BattlePC->Clnt_ShowGameResult(PlayerResults);
		}
	}
}