// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatMessage_WS.h"
#include "TextBlock.h"



bool UChatMessage_WS::Initialize()
{
	Super::Initialize();

	return true;
}

void UChatMessage_WS::SetChatMessage(FText Message)
{
	ChatMessage = Message;

	ChatTextBlock->SetText(ChatMessage);
}