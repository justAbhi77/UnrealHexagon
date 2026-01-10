// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HexTypes.h"
#include "GameFramework/Actor.h"
#include "HexGridPlacement.generated.h"

class UHexBoardConfig;
class AHexTiles;

/*
 * Grid placement actor responsible for spawning hex tiles, settlements and roads
*/
UCLASS()
class HEXAGON_API AHexGridPlacement : public AActor
{
	GENERATED_BODY()

public:
	AHexGridPlacement();

	virtual void Tick(const float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UHexBoardConfig* BoardConfig;

	// Spawned board tiles
	UPROPERTY(EditAnywhere)
	TMap<FIntPoint, AHexTiles*> PlacedTiles;

	// Spawned settlements
	UPROPERTY(EditAnywhere)
	TMap<FIntPoint, AHexTiles*> PlacedSettlements;

	// Spawned roads
	UPROPERTY(EditAnywhere)
	TMap<FIntPoint, AHexTiles*> PlacedRoads;

	FHexMouseOverInfo TileUnderMouseInfo;

	/** Cached grid Data */
	UPROPERTY(VisibleAnywhere)
	FVector GridCenterLocation;

	UPROPERTY(VisibleAnywhere)
	FVector GridBottomLeftLocation;

	UPROPERTY(VisibleAnywhere)
	FVector SnapGrid;

	UPROPERTY(VisibleAnywhere)
	FVector2D TileDiv;

	TArray<FIntPoint> HexVerticesCache;
	TArray<FIntPoint> HexEdgesCache;

	FVector2D LastMousePos;

	UPROPERTY()
	APlayerController* PlayerController;

	// Generates all board elements
	void GenerateBoardTiles(const FVector& Center, const FVector& TileSize);

	// Returns tile index under mouse cursor
	FORCEINLINE FHexHitResult GetTileUnderMouse(const FVector& Intersection);
protected:
	virtual void BeginPlay() override;
};
