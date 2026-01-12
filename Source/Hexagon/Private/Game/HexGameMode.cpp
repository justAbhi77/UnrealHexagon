// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/HexGameMode.h"
#include "Game/HexGameState.h"
#include "Game/HexPlayerState.h"

AHexGameMode::AHexGameMode()
{
}

void AHexGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AHexGameState* GS = GetGameState<AHexGameState>();
	AHexPlayerState* PS = NewPlayer->GetPlayerState<AHexPlayerState>();

	if(!GS || !PS) return;

	GS->RegisterPlayer(PS);
}
