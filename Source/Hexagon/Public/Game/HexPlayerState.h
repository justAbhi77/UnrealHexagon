// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HexTypes.h"
#include "GameFramework/PlayerState.h"
#include "HexPlayerState.generated.h"

/**
 * PlayerState for turn-based Catan logic
 * Stores only replicated player intent/state
*/
UCLASS()
class HEXAGON_API AHexPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AHexPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, VisibleAnywhere, Category="Turn")
	int32 PlayerIndex;

	UPROPERTY(ReplicatedUsing=OnRep_IsActiveTurn, VisibleAnywhere, Category="Turn")
	bool bIsActiveTurn;

	UPROPERTY(Replicated, VisibleAnywhere, Category="Turn")
	int32 SetupTurnsTakenByActivePlayer;

	UFUNCTION()
	void OnRep_IsActiveTurn();
};

