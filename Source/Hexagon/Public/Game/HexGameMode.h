// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "HexGameMode.generated.h"

/*
 * GameMode responsible only for:
 * Assigning PlayerState
 * Registering players with GameState
*/
UCLASS()
class HEXAGON_API AHexGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AHexGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
