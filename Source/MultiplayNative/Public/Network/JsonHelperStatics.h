// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "JsonHelperStatics.generated.h"

/**
 * 
 */
class FJsonObject;
class FJsonValue;

UCLASS()
class MULTIPLAYNATIVE_API UJsonHelperStatics : public UObject
{
	GENERATED_BODY()

	struct I64Struct
	{
		union
		{
			int64 Val = 0;
			int32 ValueForSerialize[2];
		};
	};	

	struct UI64Struct
	{
		union
		{
			uint64 Val = 0;
			uint32 ValueForSerialize[2];
		};
	};
	
public:
	static void SetVectorField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const FVector& VectorVal);
	static void GetVectorField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, FVector& Out_RetVector);

	static void SetIntArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const TArray<int>& Array);
	static void GetIntArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, TArray<int>& Out_RetArr);

	static void SetFloatArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const TArray<float>& Array);
	static void GetFloatArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, TArray<float>& Out_RetArr);

	static void SetStringArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const TArray<FString>& Array);
	static void GetStringArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, TArray<FString>& Out_RetArr);

	static void SetVectorArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const TArray<FVector>& Array);
	static void GetVectorArrayField(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, TArray<FVector>& Out_RetArr);

	static void SetInt64Field(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const int64& int64Val);
	static void GetInt64Field(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, int64& Out_int64Val);

	static void SetUInt64Field(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, const uint64& int64Val);
	static void GetUInt64Field(const TSharedPtr<FJsonObject> JsonObj, const FString& FIeldName, uint64& Out_int64Val);

	static FString MakeJsonString(const TSharedPtr<FJsonObject> JsonObj);
	static TSharedPtr<FJsonObject> ParseJsonString(const FString& JsonString);
};
