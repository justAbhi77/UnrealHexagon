// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/HexGameState.h"
#include "Game/HexPlayerController.h"
#include "Game/HexPlayerState.h"
#include "Net/UnrealNetwork.h"

AHexGameState::AHexGameState()
{
	bReplicates = true;
	CurrentTurnIndex = 0;
	TurnPhase = EHexTurnPhase::Setup;
	SetupRound = EHexSetupRound::First;
	bReverseTurnOrder = false;
}

void AHexGameState::RegisterPlayer(AHexPlayerState* PlayerState)
{
	if(!HasAuthority() || !PlayerState) return;

	PlayerState->PlayerIndex = TurnOrder.Num();
	TurnOrder.Add(PlayerState);

	if(TurnOrder.Num() == 1)
		PlayerState->bIsActiveTurn = true;
}

void AHexGameState::AdvanceTurn()
{
	if(!HasAuthority() || TurnOrder.Num() == 0) return;

	TurnOrder[CurrentTurnIndex]->bIsActiveTurn = false;
	TurnOrder[CurrentTurnIndex]->OnRep_IsActiveTurn();

	const int32 Direction = bReverseTurnOrder ? -1 : 1;
	CurrentTurnIndex += Direction;

	if(CurrentTurnIndex >= TurnOrder.Num() || CurrentTurnIndex < 0)
	{
		if(SetupRound == EHexSetupRound::First)
		{
			SetupRound = EHexSetupRound::Second;
			bReverseTurnOrder = true;
			CurrentTurnIndex = TurnOrder.Num() - 1;
		}
		else if(SetupRound == EHexSetupRound::Second)
		{
			TurnPhase = EHexTurnPhase::Main;
			bReverseTurnOrder = false;
			CurrentTurnIndex = 0;
		}
		else
		{
			CurrentTurnIndex = (CurrentTurnIndex + TurnOrder.Num()) % TurnOrder.Num();
		}
	}

	TurnOrder[CurrentTurnIndex]->bIsActiveTurn = true;
	TurnOrder[CurrentTurnIndex]->OnRep_IsActiveTurn();
}

AHexPlayerState* AHexGameState::GetActivePlayer() const
{
	if(TurnOrder.IsValidIndex(CurrentTurnIndex)) return TurnOrder[CurrentTurnIndex];

	return nullptr;
}

void AHexGameState::OnRep_CurrentTurnIndex()
{
}

void AHexGameState::OnRep_TurnOrder()
{
}

void AHexGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHexGameState, CurrentTurnIndex);
	DOREPLIFETIME(AHexGameState, TurnPhase);
	DOREPLIFETIME(AHexGameState, TurnOrder);
	DOREPLIFETIME(AHexGameState, SetupRound);
	DOREPLIFETIME(AHexGameState, bReverseTurnOrder);
}
