// Fill out your copyright notice in the Description page of Project Settings.

#include "MainTitle_WS.h"
#include "Button.h"
#include "EditableTextBox.h"
#include "TitlePC.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayNativeGI.h"



bool UMainTitle_WS::Initialize()
{
	Super::Initialize();

	if (IsValid(Btn_CreateRoom))
		Btn_CreateRoom->OnClicked.AddDynamic(this, &UMainTitle_WS::OnClickCreateRoom);

	if (IsValid(Btn_Join))
		Btn_Join->OnClicked.AddDynamic(this, &UMainTitle_WS::OnClickJoin);

	if (IsValid(Btn_Exit))
		Btn_Exit->OnClicked.AddDynamic(this, &UMainTitle_WS::OnClickExit);

	if (IsValid(UserNameInput))
		UserNameInput->OnTextCommitted.AddDynamic(this, &UMainTitle_WS::OnUserNameInputCommit);
	
	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		UMultiplayNativeGI* MultiplayNativeGI = Cast<UMultiplayNativeGI>(UGameplayStatics::GetGameInstance(PlayerController->GetWorld()));
		if (IsValid(MultiplayNativeGI))
		{
			UserNameInput->Text = FText::FromString(MultiplayNativeGI->MyPlayerName);
		}
	}

	return true;
}

void UMainTitle_WS::OnClickCreateRoom()
{
	ATitlePC* TitlePC = Cast<ATitlePC>(GetOwningPlayer());
	if (IsValid(TitlePC))
	{
		TitlePC->MakeGame();
	}
}

void UMainTitle_WS::OnClickJoin()
{
// 	ATitlePC* TitlePC = Cast<ATitlePC>(GetOwningPlayer());
// 	if (IsValid(TitlePC))
// 	{
// 		FString TempString = IPAddress->GetText().ToString();
// 		FName IpName = *TempString;
// 		
// 		TitlePC->JoinGame(IpName);
// 	}
}

void UMainTitle_WS::OnUserNameInputCommit(const FText& Text, ETextCommit::Type CommitMethod)
{
	UMultiplayNativeGI* MultiplayNativeGI = Cast<UMultiplayNativeGI>(UGameplayStatics::GetGameInstance(GetOwningPlayer()->GetWorld()));
	if (IsValid(MultiplayNativeGI))
	{
		MultiplayNativeGI->MyPlayerName = Text.ToString();
	}
}

void UMainTitle_WS::OnClickExit()
{
	UKismetSystemLibrary::QuitGame(GetOwningPlayer()->GetWorld(), GetOwningPlayer(), EQuitPreference::Type::Quit);
}