// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPC.h"
#include "WidgetBlueprintLibrary.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/LobbyWidget_WS.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
#include "MultiplayNativeGI.h"
#include "InGameMenu_WS.h"
#include "LobbyGM.h"
#include "LobbyGS.h"
#include "Paths.h"
#include "FileManager.h"


#define LOCTEXT_NAMESPACE "chat" 

void ALobbyPC::BeginPlay()
{
	Super::BeginPlay();

 	UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);

	if (IsLocalController())
	{
		FSoftObjectPath WidgetLoader(TEXT("WidgetBlueprint'/Game/UI/LobbyWidget_WB.LobbyWidget_WB_C'"));
		UClass* WidgetBpClass = Cast<UClass>(WidgetLoader.TryLoad());

		LobbyWidget = CreateWidget<ULobbyWidget_WS>(this, WidgetBpClass);
		if (IsValid(LobbyWidget))
		{
			LobbyWidget->AddToViewport();
		}

		UMultiplayNativeGI* MultiplayNativeGI = Cast<UMultiplayNativeGI>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (IsValid(MultiplayNativeGI))
		{
			Serv_SetPlayerName(MultiplayNativeGI->MyPlayerName);
			MultiplayNativeGI->HideLoadingScreen();
		}
	}
}

void ALobbyPC::SetupInputComponent()
{
	Super::SetupInputComponent();


	//  각 액션이나 키입력 처리를 각각 다른 함수에 맵핑할수도 있으며
	InputComponent->BindKey(EKeys::Zero, EInputEvent::IE_Pressed, this, &ALobbyPC::OnKeyInput_0);
	InputComponent->BindKey(EKeys::One, EInputEvent::IE_Pressed, this, &ALobbyPC::OnKeyInput_1);
	InputComponent->BindKey(EKeys::Two, EInputEvent::IE_Pressed, this, &ALobbyPC::OnKeyInput_2);

	InputComponent->BindKey(EKeys::Enter, EInputEvent::IE_Pressed, this, &ALobbyPC::OnInputEnter);
	InputComponent->BindKey(EKeys::X, EInputEvent::IE_Pressed, this, &ALobbyPC::OnKeyToggleMenu);

	// 아래처럼 하나의 함수에 모아서 처리하는것도 가능하니 참고
// 	FInputActionBinding InputActionBinding;
// 	InputActionBinding.ActionName = FName(TEXT("AnyKey"));
// 	InputActionBinding.KeyEvent = EInputEvent::IE_Pressed;
// 	InputActionBinding.bConsumeInput = false;
// 	InputActionBinding.ActionDelegate.BindDelegate(this, &ALobbyPC::OnKeyInput);
// 	InputComponent->AddActionBinding(InputActionBinding);
}

void ALobbyPC::Clnt_WarpRoom_Implementation(int PortNo)
{
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		//	FString IniFilePath = FPaths::LaunchDir();
		FString IniFilePath = FPaths::ProjectDir();
		IniFilePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*IniFilePath);
		IniFilePath.Append(TEXT("Test.ini"));

		FString URL = TEXT("");
		URL = UMultiplayNativeGI::ReadCustomIniPropertyString(*IniFilePath, TEXT("Connection Info"), TEXT("IP"));
		URL += FString::Printf(TEXT(":%d"), PortNo);
		
		UKismetSystemLibrary::PrintString(GetWorld(), URL);

//		UGameplayStatics::OpenLevel(World, FName(*URL));
//		World->ServerTravel(URL);
		ClientTravel(URL, ETravelType::TRAVEL_Absolute, true);
	}
}

void ALobbyPC::Client_ShowMessage_Implementation(const FString& Message)
{
	UKismetSystemLibrary::PrintString(GetWorld(), Message);
}

void ALobbyPC::Client_RoomManagerElected_Implementation()
{
// 	IsRoomManager = true;
// 	if (IsValid(LobbyWidget))
// 	{
// 		LobbyWidget->SetHost(true);
// 	}
}

// void ALobbyPC::Client_WarpRoom_Implementation(int PortNo)
// {
// 
// }

void ALobbyPC::Serv_ShowMessage_Implementation(const FString& Message)
{
	UKismetSystemLibrary::PrintString(GetWorld(), Message);
}

void ALobbyPC::Serv_SendChatMessage_Implementation(const FText& Message)
{
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
	if (!IsValid(GameModeBase))
	{
		return;
	}

	int PlayerNum = GameModeBase->GetNumPlayers();

	FText ChatMessage = FText::Format(LOCTEXT("ChatMessage", "{0} : {1}"), FText::FromString(PlayerState->GetPlayerName()), Message);

	for (int i=0 ; i<PlayerNum ; i++)
	{
		ALobbyPC* TargetPC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), i));
		if (IsValid(TargetPC))
		{
			TargetPC->Clnt_SendChatMessage(ChatMessage);
		}
	}
}

void ALobbyPC::Serv_SetPlayerName_Implementation(const FString& InPlayerName)
{
	if (HasAuthority() == false)
		return;

	ALobbyGM* LobbyGM = Cast<ALobbyGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(LobbyGM))
	{
		if (LobbyGM->FIndPlayer(InPlayerName))
		{
			Destroy();
			return;
		}

		LobbyGM->ChangeName(this, InPlayerName, false);
	}
}

void ALobbyPC::Serv_StartGame_Implementation()
{
	if (HasAuthority() == false)
		return;

	if (false == IsRoomManager)
		return;

	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
	if (!IsValid(GameModeBase))
	{
		return;
	}

	int PlayerNum = GameModeBase->GetNumPlayers();
	for (int i = 0; i<PlayerNum; i++)
	{
		ALobbyPC* TargetPC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), i));
		if (IsValid(TargetPC))
		{
			TargetPC->Clnt_ReadyToStartGame();
		}
	}


	//UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Battle")));
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		FString URL = TEXT("Battle");
		URL = URL + TEXT("?PrevLevelNumPlayers=") + FString::FromInt(PlayerNum);
		World->ServerTravel(URL);
		//			World->ServerTravel(FString(TEXT("Battle")));
	}

}

void ALobbyPC::Serv_CreateRoom_Implementation()
{
	if (HasAuthority() == false)
		return;

	if (0 != JoinedRoomNo)
		return;

	ALobbyGS* LobbyGS = Cast<ALobbyGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (!IsValid(LobbyGS))
		return;

	FString PlayerName = PlayerState->GetPlayerName();

// 	TArray<FRoomInfo>& RoomList = LobbyGS->RoomList;
// 	for (const FRoomInfo& RoomInfo : RoomList)
// 	{
// 		for (const FString& JoinedPlayerName : RoomInfo.PlayerList)
// 		{
// 			if (JoinedPlayerName == PlayerName)
// 			{
// 				return;
// 			}
// 		}
// 	}

	ALobbyGM* LobbyGM = Cast<ALobbyGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(LobbyGM))
	{
		LobbyGM->CreateBattleRoom(this);
	}
}

void ALobbyPC::Serv_JoineRoom_Implementation(int RoomNo)
{
	if (HasAuthority() == false)
		return;

	ALobbyGS* LobbyGS = Cast<ALobbyGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (!IsValid(LobbyGS))
		return;


	FRoomInfo RoomInfo;
	if (LobbyGS->GetRoomInfo(RoomNo, RoomInfo) == false)
		return;

	FString JoinPlayerName = PlayerState->GetPlayerName();
	for (const FString& PlayerName : RoomInfo.PlayerList)
	{
		if (PlayerName == JoinPlayerName)
			return;
	}

	switch (RoomInfo.BattleState)
	{
	case EBattleState::Createing:
		RoomInfo.PlayerList.Add(JoinPlayerName);
		break;

	case EBattleState::WaitingPlayers:
		RoomInfo.PlayerList.Add(JoinPlayerName);
		Clnt_WarpRoom(RoomInfo.Port);
		break;
	}
}

void ALobbyPC::Clnt_SendChatMessage_Implementation(const FText& Message)
{
	if (IsValid(LobbyWidget))
	{
		LobbyWidget->AddChatMessage(Message);
	}
}

void ALobbyPC::Clnt_ReadyToStartGame_Implementation()
{
	FSoftObjectPath WidgetLoader(TEXT("WidgetBlueprint'/Game/UI/Loading_WB.Loading_WB_C'"));
	UClass* WidgetClass = Cast<UClass>(WidgetLoader.TryLoad());
	UUserWidget* LoadingWidget = CreateWidget<UUserWidget>(this, WidgetClass);
	if (IsValid(LoadingWidget))
	{
		LoadingWidget->AddToViewport();
	}
}

void ALobbyPC::StartGame()
{
	Serv_StartGame();

// 	if (HasAuthority())
// 	{
// 		AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
// 		if (!IsValid(GameModeBase))
// 		{
// 			return;
// 		}
// 
// 		int PlayerNum = GameModeBase->GetNumPlayers();
// 		for (int i = 0; i<PlayerNum; i++)
// 		{
// 			ALobbyPC* TargetPC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), i));
// 			if (IsValid(TargetPC))
// 			{
// 				TargetPC->Clnt_ReadyToStartGame();
// 			}
// 		}
// 
// 
// 		//UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Battle")));
// 		UWorld* World = GetWorld();
// 		if (IsValid(World))
// 		{
// 			FString URL = TEXT("Battle");
// 			URL = URL + TEXT("?PrevLevelNumPlayers=") + FString::FromInt(PlayerNum);
// 			World->ServerTravel(URL);
// //			World->ServerTravel(FString(TEXT("Battle")));
// 		}
// 
// 	}	
}

void ALobbyPC::OnInputEnter()
{
	if (IsValid(LobbyWidget))
	{
		LobbyWidget->SetFocusToEditBox();
	}
}

void ALobbyPC::OnKeyInput(FKey Key)
{
	if (EKeys::Zero == Key)
	{
		ALobbyPC* LobbyPC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (IsValid(LobbyPC))
			LobbyPC->Serv_ShowMessage(FString(TEXT("플레이어 컨트롤러 0")));
	}
	else if (EKeys::One == Key)
	{
		ALobbyPC* LobbyPC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 1));
		if (IsValid(LobbyPC))
			LobbyPC->Client_ShowMessage(FString(TEXT("PlayerController 1")));
	}
	else if (EKeys::Two == Key)
	{
		ALobbyPC* LobbyPC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 2));
		if (IsValid(LobbyPC))
			LobbyPC->Client_ShowMessage(FString(TEXT("PlayerController 2")));
	}
}

void ALobbyPC::SetRoomlistVisible(bool In_Visible)
{
	if (IsValid(LobbyWidget))
	{
		LobbyWidget->SetRoomlistVisible(In_Visible);
	}	
}

void ALobbyPC::OnKeyInput_0()
{
	ALobbyPC* LobbyPC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(LobbyPC))
	{
		LobbyPC->Client_ShowMessage(FString(TEXT("PlayerController 0")));
	}
}

void ALobbyPC::OnKeyInput_1()
{
	ALobbyPC* LobbyPC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(LobbyPC))
	{
		LobbyPC->Client_ShowMessage(FString(TEXT("PlayerController 1")));
	}
}

void ALobbyPC::OnKeyInput_2()
{
	ALobbyPC* LobbyPC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(LobbyPC))
	{
		LobbyPC->Client_ShowMessage(FString(TEXT("PlayerController 2")));
	}
}

void ALobbyPC::OnKeyToggleMenu()
{
	if (!IsValid(InGameMenuWidget))
	{
		FSoftObjectPath WidgetClassloader(TEXT("WidgetBlueprint'/Game/UI/InGameMenu_WB.InGameMenu_WB_C'"));
		UClass* WidgetClass = Cast<UClass>(WidgetClassloader.TryLoad());

		InGameMenuWidget = CreateWidget<UInGameMenu_WS>(this, WidgetClass);
		InGameMenuWidget->AddToViewport();
	}

	if (!IsValid(InGameMenuWidget))
		return;

	if (InGameMenuWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		InGameMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		bShowMouseCursor = false;
	}
	else
	{
		InGameMenuWidget->SetVisibility(ESlateVisibility::Visible);
		bShowMouseCursor = true;
	}
}

#undef LOCTEXT_NAMESPACE 