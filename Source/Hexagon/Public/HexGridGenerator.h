// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HexTypes.h"
#include "GameFramework/Actor.h"
#include "HexGridGenerator.generated.h"

class UHexBoardConfig;

UCLASS()
class HEXAGON_API AHexGridGenerator : public AActor
{
	GENERATED_BODY()

public:
	AHexGridGenerator();

	virtual void Tick(const float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;

private:
	/* ===== Config ===== */
	UPROPERTY(EditAnywhere, Category="Hex Grid")
	TObjectPtr<UHexBoardConfig> BoardConfig;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

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
	TMap<FIntPoint, AHexTiles*> PlacedTiles;

	UPROPERTY()
	TMap<FIntPoint, AHexTiles*> PlacedSettlements;

	UPROPERTY()
	TMap<FIntPoint, AHexTiles*> PlacedRoads;

	/* ===== Mouse Interaction ===== */
	FHexMouseOverInfo TileUnderMouseInfo;
	FVector2D LastMousePos;

	/* ===== Generation ===== */
	void InitializeGrid();

	UFUNCTION()
	void OnRep_GridGenSeed();

	void GenerateBoardTiles(const int32 Seed);

	/* ===== Mouse ===== */
	bool TraceMouseToGrid(FVector& OutIntersection) const;
	void UpdateHoveredTile(const FVector& Intersection);
};
