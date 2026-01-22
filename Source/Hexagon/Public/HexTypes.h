
#pragma once

#include "HexTypes.generated.h"

class AHexTiles;

UENUM(BlueprintType)
enum class EHexSnapType : uint8
{
	None UMETA(DisplayName = "None"),
	Tile UMETA(DisplayName = "Tile"),
	Edge UMETA(DisplayName = "Edge"),
	Vertex UMETA(DisplayName = "Vertex")
};

UENUM(BlueprintType)
enum class EHexTileType : uint8
{
	None UMETA(DisplayName = "None"),
	Desert UMETA(DisplayName = "Desert"),
	Brick UMETA(DisplayName = "Brick"),
	Forest UMETA(DisplayName = "Forest"),
	Mountain UMETA(DisplayName = "Mountain"),
	Farm UMETA(DisplayName = "Farm"),
	Grassland UMETA(DisplayName = "Grassland"),
	Water UMETA(DisplayName = "Water"),
	Settlement UMETA(DisplayName = "Settlement"),
	Road UMETA(DisplayName = "Road")
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

UENUM(BlueprintType)
enum class EHexTurnPhase : uint8
{
	Setup UMETA(DisplayName = "Setup"),
	Main UMETA(DisplayName = "Main"),
};

UENUM(BlueprintType)
enum class EHexSetupRound : uint8
{
	First UMETA(DisplayName = "First"),
	Second UMETA(DisplayName = "Second")
};
