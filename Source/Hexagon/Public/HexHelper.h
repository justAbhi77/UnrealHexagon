// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HexHelper.generated.h"

/**
 *
 */
UCLASS()
class HEXAGON_API UHexHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Exports any struct to text using UScriptStruct::ExportText */
	UFUNCTION(BlueprintCallable, CustomThunk, meta = (CustomStructureParam = "StructData", DisplayName = "Export Struct To Text"))
	static FString Bp_ExportStructToText(const int32& StructData);

	/** Custom thunk implementation */
	DECLARE_FUNCTION(execBp_ExportStructToText);

	// ===== C++ version =====
	template<typename T>
	static FString ExportStructToText(const T& InStruct);
};

template<typename T>
FString UHexHelper::ExportStructToText(const T& InStruct)
{
	const UScriptStruct* Struct = TBaseStructure<T>::Get();

	if (!Struct) return FString();

	FString Output;
	Struct->ExportText(Output, &InStruct, nullptr, nullptr,
		PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited | PPF_IncludeTransient, nullptr);

	return Output;
}

