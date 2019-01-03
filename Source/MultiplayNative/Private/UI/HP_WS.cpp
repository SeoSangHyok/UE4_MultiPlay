// Fill out your copyright notice in the Description page of Project Settings.

#include "HP_WS.h"
#include "TextBlock.h"




void UHP_WS::SetHP(float hp)
{
	if (IsValid(HP_Text))
	{
		FString TempStr = FString::Printf(TEXT("%.2f"), hp);
		HP_Text->SetText(FText::FromString(TempStr));
	}	
}