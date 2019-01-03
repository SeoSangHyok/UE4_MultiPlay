// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMenu_WS.h"
#include "Button.h"
#include "Kismet/GameplayStatics.h"



bool UInGameMenu_WS::Initialize()
{
	Super::Initialize();

	if (IsValid(Btn_ReturnToTitle))
	{
		Btn_ReturnToTitle->OnClicked.AddDynamic(this, &UInGameMenu_WS::OnClickReturnToTitle);
	}

	if (IsValid(Btn_ExitGame))
	{
		Btn_ExitGame->OnClicked.AddDynamic(this, &UInGameMenu_WS::OnClickExitGame);
	}


	return true;
}

void UInGameMenu_WS::OnClickReturnToTitle()
{
	UGameplayStatics::OpenLevel(GetOwningPlayer()->GetWorld(), FName(TEXT("MainTitle")));
}

void UInGameMenu_WS::OnClickExitGame()
{
	UKismetSystemLibrary::QuitGame(GetOwningPlayer()->GetWorld(), GetOwningPlayer(), EQuitPreference::Type::Quit);
}