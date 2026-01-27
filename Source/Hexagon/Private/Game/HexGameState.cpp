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
	if(!HasAuthority() || TurnOrder.Num() == 0) return;

	TurnOrder[CurrentTurnIndex]->bIsActiveTurn = false;
	TurnOrder[CurrentTurnIndex]->OnRep_IsActiveTurn();

	const int32 Direction = bReverseTurnOrder ? -1 : 1;
	CurrentTurnIndex += Direction;

	if(CurrentTurnIndex >= TurnOrder.Num() || CurrentTurnIndex < 0)
	{
		if(TurnPhase == EHexTurnPhase::Setup && SetupRound == EHexSetupRound::First)
		{
			SetupRound = EHexSetupRound::Second;

			bReverseTurnOrder = true;
			CurrentTurnIndex = TurnOrder.Num() - 1;
			OnRep_SetupRound();
		}
		else if(TurnPhase == EHexTurnPhase::Setup && SetupRound == EHexSetupRound::Second)
		{
			TurnPhase = EHexTurnPhase::Main;

			bReverseTurnOrder = false;
			CurrentTurnIndex = 0;
			OnRep_TurnPhase();
		}
		else
		{
			CurrentTurnIndex = (CurrentTurnIndex + TurnOrder.Num()) % TurnOrder.Num();
		}
	}

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
