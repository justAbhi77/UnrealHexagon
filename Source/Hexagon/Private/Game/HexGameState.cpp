// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/HexGameState.h"
#include "Game/HexPlayerController.h"
#include "Game/HexPlayerState.h"
#include "Kismet/KismetSystemLibrary.h"
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
	OnRep_TurnOrder();

	if(TurnOrder.Num() == 1)
	{
		PlayerState->bIsActiveTurn = true;
		PlayerState->OnRep_IsActiveTurn();
	}
}

void AHexGameState::AdvanceTurn()
{
	if (!HasAuthority() || TurnOrder.Num() == 0) return;

	// ----- SETUP PHASE -----
	if (TurnPhase == EHexTurnPhase::Setup)
	{
		TurnOrder[CurrentTurnIndex]->SetupTurnsTakenByActivePlayer++;

		// Still the same player until they finish both setup turns
		if (TurnOrder[CurrentTurnIndex]->SetupTurnsTakenByActivePlayer < 2)
		{
			return;
		}

		// End current player's turn
		TurnOrder[CurrentTurnIndex]->bIsActiveTurn = false;
		TurnOrder[CurrentTurnIndex]->OnRep_IsActiveTurn();

		const int32 Direction = bReverseTurnOrder ? -1 : 1;
		CurrentTurnIndex += Direction;

		// Reached end of order
		if (CurrentTurnIndex >= TurnOrder.Num() || CurrentTurnIndex < 0)
		{
			// First setup round → Second (reverse order)
			if (SetupRound == EHexSetupRound::First)
			{
				SetupRound = EHexSetupRound::Second;
				bReverseTurnOrder = true;
				CurrentTurnIndex = TurnOrder.Num() - 1;
				OnRep_SetupRound();
			}
			// Second setup round → Main game
			else
			{
				TurnPhase = EHexTurnPhase::Main;
				bReverseTurnOrder = false;
				CurrentTurnIndex = 0;
				OnRep_TurnPhase();
			}
		}

		// Activate next player
		TurnOrder[CurrentTurnIndex]->bIsActiveTurn = true;
		TurnOrder[CurrentTurnIndex]->OnRep_IsActiveTurn();
		OnRep_CurrentTurnIndex();

		return;
	}

	// ----- MAIN GAME PHASE (normal single turn) -----
	TurnOrder[CurrentTurnIndex]->bIsActiveTurn = false;
	TurnOrder[CurrentTurnIndex]->OnRep_IsActiveTurn();

	CurrentTurnIndex = (CurrentTurnIndex + 1) % TurnOrder.Num();

	TurnOrder[CurrentTurnIndex]->bIsActiveTurn = true;
	TurnOrder[CurrentTurnIndex]->OnRep_IsActiveTurn();
	OnRep_CurrentTurnIndex();
}

AHexPlayerState* AHexGameState::GetActivePlayer() const
{
	if(TurnOrder.IsValidIndex(CurrentTurnIndex)) return TurnOrder[CurrentTurnIndex];

	return nullptr;
}

void AHexGameState::OnRep_TurnPhase()
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Phase Changed TurnPhase")));
	OnPhaseChanged.Broadcast();
}

void AHexGameState::OnRep_SetupRound()
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Phase Changed Setup Round")));
	OnPhaseChanged.Broadcast();
}

void AHexGameState::OnRep_CurrentTurnIndex()
{
	OnTurnIndexChanged.Broadcast();
}

void AHexGameState::OnRep_TurnOrder()
{
	OnTurnIndexChanged.Broadcast();
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
