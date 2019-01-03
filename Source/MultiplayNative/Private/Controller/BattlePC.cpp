// Fill out your copyright notice in the Description page of Project Settings.

#include "BattlePC.h"
#include "UserWidget.h"
#include "WidgetBlueprintLibrary.h"
#include "Battle_WS.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGS.h"
#include "PlayerList_WS.h"
#include "BattlePS.h"
#include "MultiplayNativeGI.h"
#include "GameFramework/SpectatorPawn.h"
#include "TimerManager.h"
#include "BattleResult_WS.h"
#include "InGameMenu_WS.h"
#include "BattleGM.h"
#include "Engine/Engine.h"
#include "Paths.h"
#include "FileManager.h"


#define LOCTEXT_NAMESPACE "chat" 

void ABattlePC::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		FSoftObjectPath WidgetLoader(TEXT("WidgetBlueprint'/Game/UI/Battle_WB.Battle_WB_C'"));
		UClass* WidgetBpClass = Cast<UClass>(WidgetLoader.TryLoad());

		BattleWS = CreateWidget<UBattle_WS>(this, WidgetBpClass);
		if (IsValid(BattleWS))
		{
			BattleWS->AddToViewport();
			bShowMouseCursor = false;
			//UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, LobbyWidget);
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);

			BattleWS->BattleGS = Cast<ABattleGS>(UGameplayStatics::GetGameState(GetWorld()));

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ABattlePC::BeginPlay()"));
			if (IsRoomManager)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("IsRoomManager is True"));
				BattleWS->SetVisibleStartBattleButton(true);
			}
		}

		UMultiplayNativeGI* MultiplayNativeGI = Cast<UMultiplayNativeGI>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (IsValid(MultiplayNativeGI))
		{
			Serv_SetPlayerName(MultiplayNativeGI->MyPlayerName);
			MultiplayNativeGI->HideLoadingScreen();
		}
	}
}

void ABattlePC::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindKey(EKeys::Enter, EInputEvent::IE_Pressed, this, &ABattlePC::OnInputEnter);
	InputComponent->BindKey(EKeys::X, EInputEvent::IE_Pressed, this, &ABattlePC::OnKeyToggleMenu);
}

void ABattlePC::OnInputEnter()
{
	if (IsValid(BattleWS))
	{
		BattleWS->SetFocusToEditBox();
	}
}

void ABattlePC::OnKeyToggleMenu()
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

void ABattlePC::OnPawnIsDead()
{
	if (HasAuthority())
	{
		ABattlePS* BattlePS = Cast<ABattlePS>(PlayerState);
		if (IsValid(BattlePS))
		{
			BattlePS->DeathCount++;
		}

 		ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(ASpectatorPawn::StaticClass(), GetPawn()->GetActorTransform());
 		Possess(SpectatorPawn);
		Clnt_NotifyPlayerDead(true);
		
		FTimerDelegate TimerCallback;
		TimerCallback.BindLambda([&]
		{
			APawn* ControlledPawn = GetPawn();
			if (IsValid(ControlledPawn))
			{
				ControlledPawn->Destroy();
			}

			AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
			if (IsValid(GameModeBase))
			{
				GameModeBase->RestartPlayer(this);
				Clnt_NotifyPlayerDead(false);
			}
		});

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, TimerCallback, DefaultRespawnTime, false);
	}
}

void ABattlePC::Serv_SendChatMessage_Implementation(const FText& Message)
{
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
	if (!IsValid(GameModeBase))
	{
		return;
	}
	
	int PlayerNum = GameModeBase->GetNumPlayers();

	FText ChatMessage = FText::Format(LOCTEXT("ChatMessage", "{0} : {1}"), FText::FromString(PlayerState->GetPlayerName()), Message);

	for (int i = 0; i<PlayerNum; i++)
	{
		ABattlePC* TargetPC = Cast<ABattlePC>(UGameplayStatics::GetPlayerController(GetWorld(), i));
		if (IsValid(TargetPC))
		{
			TargetPC->Clnt_SendChatMessage(ChatMessage);
		}
	}
}

void ABattlePC::Serv_SetPlayerName_Implementation(const FString& InPlayerName)
{
	if (HasAuthority() == false)
		return;

	ABattleGM* BattleGM = Cast<ABattleGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(BattleGM))
	{
		BattleGM->ChangeName(this, InPlayerName, false);
		BattleGM->AddPlayer(InPlayerName);
	}
}

void ABattlePC::Serv_StartBattle_Implementation()
{
	if (HasAuthority() == false)
		return;

	ABattleGM* BattleGM = Cast<ABattleGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(BattleGM))
	{
		BattleGM->StartBattle();
	}
}

void ABattlePC::Clnt_SendChatMessage_Implementation(const FText& Message)
{
	if (IsValid(BattleWS))
	{
		BattleWS->AddChatMessage(Message);
	}
}

void ABattlePC::Clnt_ReadyToStartGame_Implementation()
{
	UMultiplayNativeGI* MultiplayNativeGI = Cast<UMultiplayNativeGI>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(MultiplayNativeGI))
	{
		MultiplayNativeGI->ShowLoadingScreen();
	}

	if (IsValid(BattleWS))
	{
		BattleWS->RemoveFromParent();
	}

	if (IsValid(InGameMenuWidget))
	{
		InGameMenuWidget->RemoveFromParent();
	}

	if (GetWorldTimerManager().IsTimerActive(RespawnTimerHandler))
	{
		GetWorldTimerManager().ClearTimer(RespawnTimerHandler);
	}
}

void ABattlePC::Clnt_PlayerLogin_Implementation(const FString& PlayerName)
{
	ABattleGS* BattleGS = Cast<ABattleGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (!IsValid(BattleGS))
		return;

	if (IsValid(BattleWS))
	{
		BattleWS->PlayerList->AddPlayer(BattleGS->GetPlayerState(PlayerName));
	}
}

void ABattlePC::Clnt_PlayerLogout_Implementation(const FString& PlayerName)
{
	ABattleGS* BattleGS = Cast<ABattleGS>(UGameplayStatics::GetGameState(GetWorld()));
	if (!IsValid(BattleGS))
		return;

	if (IsValid(BattleWS))
	{
		BattleWS->PlayerList->DeleatePlayer(BattleGS->GetPlayerState(PlayerName));
	}
}

void ABattlePC::Clnt_NotifyPlayerDead_Implementation(bool InDead)
{
	if (true == InDead)
	{
		if (IsValid(BattleWS))
		{
			BattleWS->SetVIsibleFireDot(false);
		}

		if (GetWorldTimerManager().IsTimerActive(RespawnTimerHandler))
		{
			GetWorldTimerManager().ClearTimer(RespawnTimerHandler);
		}


		FTimerDelegate TimerCallback;
		TimerCallback.BindLambda([&]
		{
			if (RemainRespawnTime <= 0)
			{
				GetWorldTimerManager().ClearTimer(RespawnTimerHandler);
				if (IsValid(BattleWS))
				{
					FString RespawnMessage = FString::Printf(TEXT(""));
					BattleWS->SetRespawnMessage(RespawnMessage);
				}
			}
			else
			{
				if (IsValid(BattleWS))
				{
					FString RespawnMessage = FString::Printf(TEXT("리스폰까지 %d초 남았습니다."), RemainRespawnTime);
					BattleWS->SetRespawnMessage(RespawnMessage);
				}
			}

			RemainRespawnTime--;
		});

		GetWorldTimerManager().SetTimer(RespawnTimerHandler, TimerCallback, 1.0f, true);

		RemainRespawnTime = DefaultRespawnTime;

	}
	else
	{
		if (IsValid(BattleWS))
		{
			BattleWS->SetVIsibleFireDot(true);
		}
	}
}

void ABattlePC::Clnt_ShowGameResult_Implementation(const TArray<FPlayerResultItem>& PlayerResults)
{
	if (!IsValid(BattleWS))
		return;

	UBattleResult_WS* BattleResult = BattleWS->BattleResult;
	if (IsValid(BattleResult))
	{
		BattleResult->MakeBattleResult(PlayerResults);
		BattleResult->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABattlePC::Clnt_RoomManagerElected_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ABattlePC::Clnt_RoomManagerElected_Implementation"));

	if (IsValid(BattleWS))
		BattleWS->SetVisibleStartBattleButton(true);

	IsRoomManager = true;
}

void ABattlePC::Clnt_GotoLobby_Implementation()
{
	FName IpName(TEXT(""));

	//	FString IniFilePath = FPaths::LaunchDir();
	FString IniFilePath = FPaths::ProjectDir();
	IniFilePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*IniFilePath);
	IniFilePath.Append(TEXT("Test.ini"));

	FString IpString = TEXT("");
	IpString = UMultiplayNativeGI::ReadCustomIniPropertyString(*IniFilePath, TEXT("Connection Info"), TEXT("IP"));
	IpString.Append(TEXT(":"));
	IpString.Append(UMultiplayNativeGI::ReadCustomIniPropertyString(*IniFilePath, TEXT("Connection Info"), TEXT("PORT")));

	UKismetSystemLibrary::PrintString(GetWorld(), IpString, true, true, FLinearColor::Blue, 100.0f);
	IpName = *IpString;

	UGameplayStatics::OpenLevel(GetWorld(), IpName);
}

void ABattlePC::ShowPlayerList()
{
	if (!IsValid(BattleWS))
		return;

	BattleWS->ShowPlayerList();
}

void ABattlePC::HIdePlayerList()
{
	if (!IsValid(BattleWS))
		return;

	BattleWS->HidePlayerList();
}

#undef LOCTEXT_NAMESPACE 