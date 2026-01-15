// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/HexPlayerState.h"
#include "Net/UnrealNetwork.h"

AHexPlayerState::AHexPlayerState()
{
	bReplicates = true;
	PlayerIndex = INDEX_NONE;
	bIsActiveTurn = false;
}

void AHexPlayerState::OnRep_IsActiveTurn()
{
}

void AHexPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHexPlayerState, PlayerIndex);
	DOREPLIFETIME(AHexPlayerState, bIsActiveTurn);
}
