// Fill out your copyright notice in the Description page of Project Settings.

#include "TitlePC.h"
#include "UserWidget.h"
#include "WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TitleGM.h"




void ATitlePC::BeginPlay()
{
	Super::BeginPlay();

	FSoftObjectPath TitleWidgetLoader(TEXT("WidgetBlueprint'/Game/UI/MainTitle_WB.MainTitle_WB_C'"));
	UClass* TitleWidgetClass = Cast<UClass>(TitleWidgetLoader.TryLoad());

	FSoftObjectPath TitleWidgetMSLoader(TEXT("WidgetBlueprint'/Game/UI/MainTitleMS_WB.MainTitleMS_WB_C'"));
	UClass* TitleWidgetMSClass = Cast<UClass>(TitleWidgetMSLoader.TryLoad());

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerController))
	{
		ATitleGM* TitleGM = Cast<ATitleGM>(UGameplayStatics::GetGameMode(GetWorld()));
		if (IsValid(TitleGM))
		{
			if (TitleGM->UseMasterServMode)
			{
				TItleWidget = CreateWidget<UUserWidget>(PlayerController, TitleWidgetMSClass);
			}
			else
			{
				TItleWidget = CreateWidget<UUserWidget>(PlayerController, TitleWidgetClass);
			}
		}

		if (IsValid(TItleWidget))
		{
			TItleWidget->AddToViewport();
			PlayerController->bShowMouseCursor = true;
			UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController, TItleWidget);
		}
	}
}

void ATitlePC::MakeGame()
{
	FSoftObjectPath WidgetLoader(TEXT("WidgetBlueprint'/Game/UI/Loading_WB.Loading_WB_C'"));
	UClass* WidgetClass = Cast<UClass>(WidgetLoader.TryLoad());
	UUserWidget* LoadingWidget = CreateWidget<UUserWidget>(this, WidgetClass);
	if (IsValid(LoadingWidget))
	{
		LoadingWidget->AddToViewport();
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Lobby")), true, FString(TEXT("Listen")));
}

void ATitlePC::JoinGame(FName IpAddress, FString OptionString)
{
	FSoftObjectPath WidgetLoader(TEXT("WidgetBlueprint'/Game/UI/Loading_WB.Loading_WB_C'"));
	UClass* WidgetClass = Cast<UClass>(WidgetLoader.TryLoad());
	UUserWidget* LoadingWidget = CreateWidget<UUserWidget>(this, WidgetClass);
	if (IsValid(LoadingWidget))
	{
		LoadingWidget->AddToViewport();
	}

	//ClientTravel(IpAddress.ToString(), ETravelType::TRAVEL_Absolute);
	UGameplayStatics::OpenLevel(GetWorld(), IpAddress);
}