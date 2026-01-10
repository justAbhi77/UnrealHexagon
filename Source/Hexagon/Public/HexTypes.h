
#pragma once

#include "HexTypes.generated.h"

class AHexTiles;

UENUM()
enum class EHexSnapType : uint8
{
	None,
	Tile,
	Edge,
	Vertex
};

UENUM(BlueprintType)
enum EHexTileType : uint8
{
	None,
	Desert,
	Brick,
	Forest,
	Mountain,
	Farm,
	Grassland,
	Water,
};

USTRUCT(BlueprintType)
struct FHexHitResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bHit = false;

	UPROPERTY(BlueprintReadOnly)
	EHexSnapType SnapType = EHexSnapType::None;

	UPROPERTY(BlueprintReadOnly)
	FIntPoint ClosestIndex = FIntPoint(-1, -1);
};

USTRUCT(BlueprintType)
struct FHexMouseOverInfo
{
	GENERATED_BODY()

	UPROPERTY()
	AHexTiles* Actor = nullptr;

	FVector2D Index = FVector2D(-1, -1);
};