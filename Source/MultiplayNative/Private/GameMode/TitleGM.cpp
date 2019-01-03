// Fill out your copyright notice in the Description page of Project Settings.

#include "TitleGM.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "TitlePC.h"
#include "PacketDefine.h"
#include "Engine/Engine.h"


DEFINE_LOG_CATEGORY(LogTitleGM);

ATitleGM::ATitleGM()
{
	PlayerControllerClass = ATitlePC::StaticClass();
}

void ATitleGM::StartPlay()
{
	Super::StartPlay();
}