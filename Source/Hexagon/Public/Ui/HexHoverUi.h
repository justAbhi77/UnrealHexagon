// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HexHoverUi.generated.h"

class UHexDataUi;

/**
 *
 */
UCLASS()
class HEXAGON_API UHexHoverUi : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true", BindWidget))
	TObjectPtr<UHexDataUi> TileType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true", BindWidget))
	TObjectPtr<UHexDataUi> DiceNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true", BindWidget))
	TObjectPtr<UHexDataUi> Owner;
};
