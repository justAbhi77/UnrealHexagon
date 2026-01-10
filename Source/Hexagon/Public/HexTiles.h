// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HexTypes.h"
#include "GameFramework/Actor.h"
#include "HexTiles.generated.h"

UCLASS()
class HEXAGON_API AHexTiles : public AActor
{
	GENERATED_BODY()

public:
	AHexTiles();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	FIntPoint TileCoordinates;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	FLinearColor TileColor;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnTileHighlighted();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnTileUnHighlighted();

	void OnHighlight();

	void OnUnHighlight();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	TEnumAsByte<EHexTileType> TileType = None;
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
