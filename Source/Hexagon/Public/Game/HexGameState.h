// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HexTypes.h"
#include "GameFramework/GameState.h"
#include "HexGameState.generated.h"

class AHexPlayerState;

/**
 *
 */
UCLASS()
class HEXAGON_API AHexGameState : public AGameState
{
	GENERATED_BODY()

public:
	AHexGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentTurnIndex, VisibleAnywhere, Category="Turn")
	int32 CurrentTurnIndex;

	UPROPERTY(Replicated, VisibleAnywhere, Category="Turn")
	EHexTurnPhase TurnPhase;

	UPROPERTY(ReplicatedUsing=OnRep_TurnOrder)
	TArray<AHexPlayerState*> TurnOrder;

	void RegisterPlayer(AHexPlayerState* PlayerState);

	void AdvanceTurn();

	UFUNCTION()
	void OnRep_CurrentTurnIndex();

	UFUNCTION()
	void OnRep_TurnOrder();

	AHexPlayerState* GetActivePlayer() const;

	UPROPERTY(Replicated)
	EHexSetupRound SetupRound;

	UPROPERTY(Replicated)
	bool bReverseTurnOrder;
};
