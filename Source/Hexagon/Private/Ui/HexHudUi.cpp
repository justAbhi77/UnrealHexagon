// Fill out your copyright notice in the Description page of Project Settings.


#include "Ui/HexHudUi.h"
#include "Components/Button.h"

void UHexHudUi::NativeConstruct()
{
	Super::NativeConstruct();
	EndTurnButton->OnClicked.AddDynamic(this, &UHexHudUi::EndTurnButtonClicked);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UHexHudUi::EndTurnButtonClicked()
{
	OnEndTurnPressed.Broadcast();
}
