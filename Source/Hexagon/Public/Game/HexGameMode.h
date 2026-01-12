// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HexGameMode.generated.h"

/*
 * GameMode responsible only for:
 * Assigning PlayerState
 * Registering players with GameState
*/
UCLASS()
class HEXAGON_API AHexGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHexGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
};
