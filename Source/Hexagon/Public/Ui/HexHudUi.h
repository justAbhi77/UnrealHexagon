// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HexHudUi.generated.h"

class UButton;
class UHexDataUi;

/**
 *
 */
UCLASS()
class HEXAGON_API UHexHudUi : public UUserWidget
{
	GENERATED_BODY()
public:

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true", BindWidget))
	TObjectPtr<UHexDataUi> TurnText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true", BindWidget))
	TObjectPtr<UHexDataUi> TurnPhase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true", BindWidget))
	TObjectPtr<UButton> EndTurnButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true", BindWidget))
	TObjectPtr<UHexDataUi> CurrentSelectedTile;
};
