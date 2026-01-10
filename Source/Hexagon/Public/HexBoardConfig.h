// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HexTypes.h"
#include "Engine/DataAsset.h"
#include "HexBoardConfig.generated.h"

class AHexTiles;

/**
 *
 */
UCLASS()
class HEXAGON_API UHexBoardConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 HexagonGridSize = 3;

	UPROPERTY(EditAnywhere)
	FVector GridTileSize = FVector(115.f, 100.f, 10.f);

	// Tile mesh scale reference
	UPROPERTY(EditAnywhere)
	FVector TileMeshSize = FVector(115.f, 100.f, 10.f);

	// Settlement mesh scale reference
	UPROPERTY(EditAnywhere)
	FVector SettlementMeshSize = FVector(25.f, 25.f, 3.f);

	UPROPERTY(EditAnywhere)
	float RoadMeshSizeY = 5.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AHexTiles> HexTileClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AHexTiles> SettlementTileClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AHexTiles> RoadTileClass;

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EHexTileType>, int32> TileTypeDistribution = {
		{EHexTileType::Desert, 1},
		{EHexTileType::Brick, 3},
		{EHexTileType::Forest, 4},
		{EHexTileType::Mountain, 3},
		{EHexTileType::Farm, 4},
		{EHexTileType::Grassland, 4},
	};

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EHexTileType>, FLinearColor> TileTypeColor =
	{
		{ EHexTileType::Desert,     FLinearColor(0.85f, 0.80f, 0.55f, 1.f) }, // sand
		{ EHexTileType::Brick,      FLinearColor(0.72f, 0.25f, 0.20f, 1.f) }, // clay
		{ EHexTileType::Forest,     FLinearColor(0.10f, 0.45f, 0.15f, 1.f) }, // wood
		{ EHexTileType::Mountain,   FLinearColor(0.45f, 0.45f, 0.48f, 1.f) }, // ore
		{ EHexTileType::Farm,       FLinearColor(0.95f, 0.85f, 0.30f, 1.f) }, // wheat
		{ EHexTileType::Grassland,  FLinearColor(0.35f, 0.70f, 0.30f, 1.f) }, // sheep
	};

	// Returns total grid tile count
	FVector2D GetGridTileCount() const
	{
		const int TileSize = HexagonGridSize * 2 - 1;
		return FVector2D(TileSize, TileSize);
	}

	TArray<EHexTileType> GetShuffledTileTypes() const;
};
