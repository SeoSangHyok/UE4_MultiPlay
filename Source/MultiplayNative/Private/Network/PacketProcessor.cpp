// Fill out your copyright notice in the Description page of Project Settings.

#include "PacketProcessor.h"
#include "JsonHelperStatics.h"
#include "JsonObject.h"
#include "PacketDefine.h"


UPacketProcessor::UPacketProcessor()
{
	PacketParseProcMap.AddDefaulted(static_cast<int>(EPacketMainType::MainTypeCount));

	PacketParseProcMap[static_cast<int>(EPacketMainType::Lobby)].AddDefaulted(static_cast<int>(EPacketLobbySubType::LobbySubTypeCount));
}

void UPacketProcessor::ParsePacketData(const FString& DataString)
{
	TSharedPtr<FJsonObject> JsonObject = UJsonHelperStatics::ParseJsonString(DataString);
	if (!JsonObject.IsValid())
		return;

	TSharedPtr<FJsonObject> PacketHaderJsonObj = JsonObject->GetObjectField(FPacketHeaderType::GetJsonHeaderString());
	if (!PacketHaderJsonObj.IsValid())
		return;

	FPacketHeaderType PacketHeader;
	if (PacketHeader.ParseJsonObject(PacketHaderJsonObj) == false)
		return;

// 	if (PacketParseProcMap.Num() < PacketHeader.MainType)
// 	{
// 		if (PacketParseProcMap[PacketHeader.MainType].Num() < PacketHeader.SubType)
// 		{
// 			FPacketParseProc& PacketParseProc = PacketParseProcMap[PacketHeader.MainType][PacketHeader.SubType];
// 			if (PacketParseProc.IsBound())
// 			{
// 				FParsedDataInfo ParsedDataInfo;
// 				ParsedDataInfo.PacketHaderJsonObj = JsonObject;
// 				PacketParseProc.Broadcast(ParsedDataInfo);
// 			}
// 		}
// 	}
}