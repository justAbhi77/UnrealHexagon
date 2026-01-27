// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HexTypes.h"
#include "GameFramework/GameState.h"
#include "HexGameState.generated.h"

class AHexPlayerController;
class AHexPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnIndexChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhaseChanged);

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

	UPROPERTY(ReplicatedUsing=OnRep_TurnPhase, VisibleAnywhere, Category="Turn")
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

	UPROPERTY(ReplicatedUsing=OnRep_SetupRound, VisibleAnywhere, Category="Turn")
	EHexSetupRound SetupRound;

	UPROPERTY(Replicated)
	bool bReverseTurnOrder;

	UPROPERTY(BlueprintAssignable)
	FOnTurnIndexChanged OnTurnIndexChanged;

	UFUNCTION()
	void OnRep_TurnPhase();

	UFUNCTION()
	void OnRep_SetupRound();

	UPROPERTY(BlueprintAssignable)
	FOnPhaseChanged OnPhaseChanged;
};
