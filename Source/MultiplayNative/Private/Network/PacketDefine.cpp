// Fill out your copyright notice in the Description page of Project Settings.

#include "PacketDefine.h"
#include "JsonObject.h"
#include "JsonWriter.h"
#include "JsonSerializer.h"
#include "JsonHelperStatics.h"




FTestPacket::FTestPacket()
{
	StrVal.Empty();
	StrArr.Reset();
	IntVal = 0;
	IntArr.Reset();
	VectorVal = FVector::ZeroVector;
	VectorArr.Reset();
}

bool FTestPacket::ParseJsonString(FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObj) == false || JsonObj.IsValid() == false)
		return false;

	StrVal = JsonObj->GetStringField(TEXT("StrVal"));
	UJsonHelperStatics::GetStringArrayField(JsonObj, TEXT("StrArr"), StrArr);

	IntVal = JsonObj->GetIntegerField(TEXT("IntVal"));
	UJsonHelperStatics::GetIntArrayField(JsonObj, TEXT("IntArr"), IntArr);

	UJsonHelperStatics::GetVectorField(JsonObj, TEXT("VectorVal"), VectorVal);
	UJsonHelperStatics::GetVectorArrayField(JsonObj, TEXT("VectorArr"), VectorArr);

	return true;
}

TSharedPtr<FJsonObject> FPacketHeaderType::MakeJsonObject()
{
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	JsonObj->SetStringField(TEXT("HeaderString"), GetJsonHeaderString());
	UJsonHelperStatics::SetUInt64Field(JsonObj, TEXT("PacketData"), PacketData);

	return JsonObj;
}

bool FPacketHeaderType::ParseJsonObject(const TSharedPtr<FJsonObject>& JsonObject)
{
	if (GetJsonHeaderString() != JsonObject->GetStringField(TEXT("HeaderString")))
		return false;

	UJsonHelperStatics::GetUInt64Field(JsonObject, TEXT("PacketData"), PacketData);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// FPacketRoomCreateComplete
TSharedPtr<FJsonObject> FPacketRoomCreateComplete::MakeJsonObject()
{
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	JsonObj->SetStringField(TEXT("HeaderString"), GetJsonHeaderString());
	JsonObj->SetNumberField(TEXT("RoomNo"), RoomNo);

	return JsonObj;
}

bool FPacketRoomCreateComplete::ParseJsonObject(const TSharedPtr<FJsonObject>& JsonObject)
{
	if (GetJsonHeaderString() != JsonObject->GetStringField(TEXT("HeaderString")))
		return false;

	RoomNo = JsonObject->GetNumberField(TEXT("RoomNo"));

	return true;
}

//////////////////////////////////////////////////////////////////////////
// FPacketChageRoomInfo
TSharedPtr<FJsonObject> FPacketChageRoomInfo::MakeJsonObject()
{
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	JsonObj->SetStringField(TEXT("HeaderString"), GetJsonHeaderString());
	JsonObj->SetNumberField(TEXT("RoomNo"), RoomNo);
	JsonObj->SetNumberField(TEXT("RoomState"), RoomState);
	UJsonHelperStatics::SetStringArrayField(JsonObj, TEXT("PlayerList"), PlayerList);

	return JsonObj;
}

bool FPacketChageRoomInfo::ParseJsonObject(const TSharedPtr<FJsonObject>& JsonObject)
{
	if (GetJsonHeaderString() != JsonObject->GetStringField(TEXT("HeaderString")))
		return false;

	RoomNo = JsonObject->GetNumberField(TEXT("RoomNo"));
	RoomState = JsonObject->GetNumberField(TEXT("RoomState"));
	UJsonHelperStatics::GetStringArrayField(JsonObject, TEXT("PlayerList"), PlayerList);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// FPacketDeleteRoom
TSharedPtr<FJsonObject> FPacketDeleteRoom::MakeJsonObject()
{
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	JsonObj->SetStringField(TEXT("HeaderString"), GetJsonHeaderString());
	JsonObj->SetNumberField(TEXT("RoomNo"), RoomNo);

	return JsonObj;
}

bool FPacketDeleteRoom::ParseJsonObject(const TSharedPtr<FJsonObject>& JsonObject)
{
	if (GetJsonHeaderString() != JsonObject->GetStringField(TEXT("HeaderString")))
		return false;

	RoomNo = JsonObject->GetNumberField(TEXT("RoomNo"));

	return true;
}

//////////////////////////////////////////////////////////////////////////
// PacketBattleUserLogout
TSharedPtr<FJsonObject> FPacketBattleUserLogout::MakeJsonObject()
{
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	JsonObj->SetStringField(TEXT("HeaderString"), GetJsonHeaderString());
	JsonObj->SetNumberField(TEXT("RoomNo"), RoomNo);
	JsonObj->SetStringField(TEXT("LogoutUserName"), LogoutUserName);


	return JsonObj;
}

bool FPacketBattleUserLogout::ParseJsonObject(const TSharedPtr<FJsonObject>& JsonObject)
{
	if (GetJsonHeaderString() != JsonObject->GetStringField(TEXT("HeaderString")))
		return false;

	RoomNo = JsonObject->GetNumberField(TEXT("RoomNo"));
	LogoutUserName = JsonObject->GetStringField(TEXT("LogoutUserName"));

	return true;
}