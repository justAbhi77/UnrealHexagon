// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HexTypes.h"
#include "GameFramework/Actor.h"
#include "HexGridGenerator.generated.h"

class AHexPlayerController;
class UHexBoardConfig;

UCLASS()
class HEXAGON_API AHexGridGenerator : public AActor
{
	GENERATED_BODY()

public:
	AHexGridGenerator();

	virtual void Tick(const float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FHexHitResult SendCurrentHoverSelection() const;
protected:
	virtual void BeginPlay() override;

private:
	/* ===== Config ===== */
	UPROPERTY(EditAnywhere, Category="Hex Grid")
	TObjectPtr<UHexBoardConfig> BoardConfig;

	UPROPERTY()
	TObjectPtr<AHexPlayerController> PlayerController;

	/* ===== Grid Cache ===== */
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_GridGenSeed, Category="Hex Grid")
	int32 GridGenSeed;

	UPROPERTY(VisibleAnywhere, Category="Hex Grid")
	FVector GridCenterLocation;

	UPROPERTY(VisibleAnywhere, Category="Hex Grid")
	FVector GridBottomLeftLocation;

	UPROPERTY(VisibleAnywhere, Category="Hex Grid")
	FVector SnapGrid;

	UPROPERTY(VisibleAnywhere, Category="Hex Grid")
	FVector2D TileDiv;

	/* ===== Placed Actors ===== */
	UPROPERTY()
	TMap<FIntPoint, AHexTiles*> SpawnedHexTiles;

	/* ===== Mouse Interaction ===== */
	FHexMouseOverInfo TileUnderMouseInfo;
	FVector2D LastMousePos;

	/* ===== Generation ===== */
	void InitializeGrid();

	UFUNCTION()
	void OnRep_GridGenSeed();

	/* ===== Mouse ===== */
	void UpdateHoveredTile(const FVector& Intersection);
	void StartGenerateBoard(int32 Seed);
	void SpawnNextHex();
	void SpawnSingleHex(const FIntPoint& TileIndex);

	// Coroutine
	FIntPoint GridSize;
	FTimerHandle SpawnTimer;
	int32 CurrentX = 0;
	int32 CurrentYIndex = 0;
	TArray<int32> CurrentColumnRows;
	FHexHitResult MouseHoverResult;

	TArray<EHexTileType> TileTypes;
	TArray<int32> TileDiceNumbers;

	int32 TileTypeIndex = 0;
};
