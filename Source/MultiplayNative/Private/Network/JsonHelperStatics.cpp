// Fill out your copyright notice in the Description page of Project Settings.

#include "JsonHelperStatics.h"
#include "JsonObject.h"
#include "JsonWriter.h"
#include "JsonSerializer.h"




void UJsonHelperStatics::SetVectorField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const FVector& VectorVal)
{
	if (!JsonObj.IsValid())
		return;

	TSharedPtr<FJsonObject> JsonVectorObj = MakeShareable(new FJsonObject());
	if (JsonVectorObj.IsValid())
	{
		JsonVectorObj->SetNumberField(TEXT("X"), VectorVal.X);
		JsonVectorObj->SetNumberField(TEXT("Y"), VectorVal.Y);
		JsonVectorObj->SetNumberField(TEXT("Z"), VectorVal.Z);
		JsonObj->SetObjectField(FIeldName, JsonVectorObj);
	}
}

void UJsonHelperStatics::GetVectorField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, FVector& Out_RetVector)
{
	if (!JsonObj.IsValid())
		return;

	const TSharedPtr<FJsonObject> JsonVectorObj = JsonObj->GetObjectField(FIeldName);


	if (!JsonVectorObj.IsValid())
		return;

	Out_RetVector.X = JsonVectorObj->GetNumberField(TEXT("X"));
	Out_RetVector.Y = JsonVectorObj->GetNumberField(TEXT("Y"));
	Out_RetVector.Z = JsonVectorObj->GetNumberField(TEXT("Z"));
}

void UJsonHelperStatics::SetIntArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const TArray<int>& Array)
{
	if (!JsonObj.IsValid())
		return;

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	for (const int ArrElem : Array)
	{
		JsonArray.Add(MakeShareable(new FJsonValueNumber(ArrElem)));
	}
	JsonObj->SetArrayField(FIeldName, JsonArray);
}

void UJsonHelperStatics::GetIntArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, TArray<int>& Out_RetArr)
{
	if (!JsonObj.IsValid())
		return;

	Out_RetArr.Reset();

	const TArray<TSharedPtr<FJsonValue>>& JsonValueArray = JsonObj->GetArrayField(FIeldName);

	for (const TSharedPtr<FJsonValue>& JsonValueElem : JsonValueArray)
	{
		if (JsonValueElem->Type == EJson::Number)
		{
			Out_RetArr.Add(JsonValueElem->AsNumber());
		}
	}
}

void UJsonHelperStatics::SetFloatArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const TArray<float>& Array)
{
	if (!JsonObj.IsValid())
		return;

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	for (const float ArrElem : Array)
	{
		JsonArray.Add(MakeShareable(new FJsonValueNumber(ArrElem)));
	}
	JsonObj->SetArrayField(FIeldName, JsonArray);
}

void UJsonHelperStatics::GetFloatArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, TArray<float>& Out_RetArr)
{
	if (!JsonObj.IsValid())
		return;

	Out_RetArr.Reset();

	const TArray<TSharedPtr<FJsonValue>>& JsonValueArray = JsonObj->GetArrayField(FIeldName);

	for (const TSharedPtr<FJsonValue>& JsonValueElem : JsonValueArray)
	{
		if (JsonValueElem->Type == EJson::Number)
		{
			Out_RetArr.Add(JsonValueElem->AsNumber());
		}
	}
}

void UJsonHelperStatics::SetStringArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const TArray<FString>& Array)
{
	if (!JsonObj.IsValid())
		return;

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	for (const FString ArrElem : Array)
	{
		JsonArray.Add(MakeShareable(new FJsonValueString(ArrElem)));
	}
	JsonObj->SetArrayField(FIeldName, JsonArray);
}

void UJsonHelperStatics::GetStringArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, TArray<FString>& Out_RetArr)
{
	if (!JsonObj.IsValid())
		return;

	Out_RetArr.Reset();

	const TArray<TSharedPtr<FJsonValue>>& JsonValueArray = JsonObj->GetArrayField(FIeldName);

	for (const TSharedPtr<FJsonValue>& JsonStrValArrElem : JsonValueArray)
	{
		if (JsonStrValArrElem->Type == EJson::String)
		{
			Out_RetArr.Add(JsonStrValArrElem->AsString());
		}
	}
}

void UJsonHelperStatics::SetVectorArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const TArray<FVector>& Array)
{
	if (!JsonObj.IsValid())
		return;

	TArray<TSharedPtr<FJsonValue>> JsonVectorArr;
	for (const auto& ArrElem : Array)
	{
		TSharedPtr<FJsonObject> JsonVectorObj = MakeShareable(new FJsonObject());
		if (JsonVectorObj.IsValid())
		{
			JsonVectorObj->SetNumberField(TEXT("X"), ArrElem.X);
			JsonVectorObj->SetNumberField(TEXT("Y"), ArrElem.Y);
			JsonVectorObj->SetNumberField(TEXT("Z"), ArrElem.Z);
			JsonVectorArr.Add(MakeShareable(new FJsonValueObject(JsonVectorObj)));
		}
	}
	JsonObj->SetArrayField(FIeldName, JsonVectorArr);
}

void UJsonHelperStatics::GetVectorArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, TArray<FVector>& Out_RetArr)
{
	if (!JsonObj.IsValid())
		return;

	const TArray<TSharedPtr<FJsonValue>>& JsonValueArray = JsonObj->GetArrayField(FIeldName);

	for (const TSharedPtr<FJsonValue>& JsonValueElem : JsonValueArray)
	{
		if (JsonValueElem.IsValid() && JsonValueElem->Type == EJson::Object)
		{
			TSharedPtr<FJsonObject> JsonVectorObj = JsonValueElem->AsObject();
			if (JsonVectorObj.IsValid())
			{
				FVector Vector;
				Vector.X = JsonVectorObj->GetNumberField(TEXT("X"));
				Vector.Y = JsonVectorObj->GetNumberField(TEXT("Y"));
				Vector.Z = JsonVectorObj->GetNumberField(TEXT("Z"));
				Out_RetArr.Add(Vector);
			}
		}
	}
}

void UJsonHelperStatics::SetInt64Field(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const int64& int64Val)
{
	if (!JsonObj.IsValid())
		return;

	I64Struct I64StructVal;
	I64StructVal.Val = int64Val;

	TSharedPtr<FJsonObject> JsonInt64Obj = MakeShareable(new FJsonObject());
	if (JsonInt64Obj.IsValid())
	{
		JsonInt64Obj->SetNumberField(TEXT("I64JsonStruct.ValueForSerialize_1"), I64StructVal.ValueForSerialize[0]);
		JsonInt64Obj->SetNumberField(TEXT("I64JsonStruct.ValueForSerialize_2"), I64StructVal.ValueForSerialize[1]);
		JsonObj->SetObjectField(FIeldName, JsonInt64Obj);
	}
}

void UJsonHelperStatics::GetInt64Field(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, int64& Out_int64Val)
{
	if (!JsonObj.IsValid())
		return;

	const TSharedPtr<FJsonObject> JsonInt64Obj = JsonObj->GetObjectField(FIeldName);

	if (!JsonInt64Obj.IsValid())
		return;

	I64Struct I64StructVal;
	I64StructVal.ValueForSerialize[0] = JsonInt64Obj->GetNumberField(TEXT("I64JsonStruct.ValueForSerialize_1"));
	I64StructVal.ValueForSerialize[1] = JsonInt64Obj->GetNumberField(TEXT("I64JsonStruct.ValueForSerialize_2"));
	Out_int64Val = I64StructVal.Val;
}

void UJsonHelperStatics::SetUInt64Field(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const uint64& int64Val)
{
	if (!JsonObj.IsValid())
		return;

	UI64Struct UI64StructVal;
	UI64StructVal.Val = int64Val;

	TSharedPtr<FJsonObject> JsonUInt64Obj = MakeShareable(new FJsonObject());
	if (JsonUInt64Obj.IsValid())
	{
		JsonUInt64Obj->SetNumberField(TEXT("UI64StructVal.ValueForSerialize_1"), UI64StructVal.ValueForSerialize[0]);
		JsonUInt64Obj->SetNumberField(TEXT("UI64StructVal.ValueForSerialize_2"), UI64StructVal.ValueForSerialize[1]);
		JsonObj->SetObjectField(FIeldName, JsonUInt64Obj);
	}
}

void UJsonHelperStatics::GetUInt64Field(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, uint64& Out_int64Val)
{
	if (!JsonObj.IsValid())
		return;

	const TSharedPtr<FJsonObject> JsonUInt64Obj = JsonObj->GetObjectField(FIeldName);

	if (!JsonUInt64Obj.IsValid())
		return;

	UI64Struct UI64StructVal;
	UI64StructVal.ValueForSerialize[0] = JsonUInt64Obj->GetNumberField(TEXT("UI64StructVal.ValueForSerialize_1"));
	UI64StructVal.ValueForSerialize[1] = JsonUInt64Obj->GetNumberField(TEXT("UI64StructVal.ValueForSerialize_2"));
	Out_int64Val = UI64StructVal.Val;
}

FString UJsonHelperStatics::MakeJsonString(const TSharedPtr<FJsonObject> JsonObj)
{
	FString RetString;
	RetString.Empty();

	if (!JsonObj.IsValid())
		return RetString;

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RetString);
	FJsonSerializer::Serialize(JsonObj.ToSharedRef(), Writer);

	return RetString;
}

TSharedPtr<FJsonObject> UJsonHelperStatics::ParseJsonString(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObj) == false || JsonObj.IsValid() == false)
		return nullptr;

	return JsonObj;
}