// Fill out your copyright notice in the Description page of Project Settings.

#include "MainTitleMS_WS.h"
#include "Button.h"
#include "EditableTextBox.h"
#include "TitlePC.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayNativeGI.h"
#include "Paths.h"
#include "FileManager.h"



bool UMainTitleMS_WS::Initialize()
{
	Super::Initialize();

	if (IsValid(Btn_Join))
		Btn_Join->OnClicked.AddDynamic(this, &UMainTitleMS_WS::OnClickJoin);

	if (IsValid(Btn_Exit))
		Btn_Exit->OnClicked.AddDynamic(this, &UMainTitleMS_WS::OnClickExit);

	if (IsValid(UserNameInput))
		UserNameInput->OnTextCommitted.AddDynamic(this, &UMainTitleMS_WS::OnUserNameInputCommit);

	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		UMultiplayNativeGI* MultiplayNativeGI = Cast<UMultiplayNativeGI>(UGameplayStatics::GetGameInstance(PlayerController->GetWorld()));
		if (IsValid(MultiplayNativeGI))
		{
			FString PlayerName = MultiplayNativeGI->MyPlayerName;

			UserNameInput->Text = FText::FromString(PlayerName);
			if (PlayerName.Len() == 0)
				Btn_Join->SetIsEnabled(false);
			else
				Btn_Join->SetIsEnabled(true);
		}
	}

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

	return true;
}

void UMainTitleMS_WS::OnClickJoin()
{
	ATitlePC* TitlePC = Cast<ATitlePC>(GetOwningPlayer());
	if (IsValid(TitlePC) && IsValid(UserNameInput))
	{
//		UserNameInput->Text
//		FText::ToString()

		//FName IpName(TEXT("127.0.0.1:7777"));




//		FName IpName(TEXT("54.180.93.226:7777"));
		FString OptionString = FString::Printf(TEXT("?PlayerName=%s"), *UserNameInput->Text.ToString());

		TitlePC->JoinGame(IpName, OptionString);
	}
}

void UMainTitleMS_WS::OnUserNameInputCommit(const FText& Text, ETextCommit::Type CommitMethod)
{
	UMultiplayNativeGI* MultiplayNativeGI = Cast<UMultiplayNativeGI>(UGameplayStatics::GetGameInstance(GetOwningPlayer()->GetWorld()));
	FString PlayerName = Text.ToString();

	if (IsValid(MultiplayNativeGI))
	{
		MultiplayNativeGI->MyPlayerName = PlayerName;
	}

	if (PlayerName.Len() == 0)
		Btn_Join->SetIsEnabled(false);
	else
		Btn_Join->SetIsEnabled(true);
}

void UMainTitleMS_WS::OnClickExit()
{
	UKismetSystemLibrary::QuitGame(GetOwningPlayer()->GetWorld(), GetOwningPlayer(), EQuitPreference::Type::Quit);
}



