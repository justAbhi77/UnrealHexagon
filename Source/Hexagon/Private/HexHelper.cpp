// Fill out your copyright notice in the Description page of Project Settings.


#include "HexHelper.h"

DEFINE_FUNCTION(UHexHelper::execBp_ExportStructToText)
{
	// Step 1: Get the struct property
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	const FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
	const void* StructPtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	FString Output;

	if (StructProperty && StructPtr)
	{
		if (const UScriptStruct* Struct = StructProperty->Struct)
		{
			Struct->ExportText(Output, StructPtr, nullptr, nullptr,
				PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited | PPF_IncludeTransient, nullptr);
		}
	}

	*static_cast<FString*>(RESULT_PARAM) = Output;
}
