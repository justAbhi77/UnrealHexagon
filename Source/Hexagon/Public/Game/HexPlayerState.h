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

	UPROPERTY(Replicated, VisibleAnywhere, Category="Turn")
	bool bIsActiveTurn;

	UPROPERTY(ReplicatedUsing=OnRep_LastSelection, VisibleAnywhere, Category="Turn")
	FHexHitResult LastSelection;

	UFUNCTION()
	void OnRep_LastSelection();
};

