// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HexTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HexMath.generated.h"

class AHexPlayerState;
class AHexGameState;
class UHexBoardConfig;

constexpr float One6 = 1.f / 6.f;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FValidHexRow, int32, Row);

/**
 *
 */
UCLASS()
class HEXAGON_API UHexMath : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static FVector CalculateSnapGrid(const FVector& TileSize);

	UFUNCTION(BlueprintCallable)
	static FVector2D CalculateTileDiv(const FVector& TileSize);

	// Snaps a vector to grid size
	UFUNCTION(BlueprintCallable)
	static FVector SnapVectorToVector(const FVector& VectorToSnap, const FVector& GridVector);

	// Returns hexagon vertices
	UFUNCTION(BlueprintCallable)
	static void GetHexagonVertices(const FIntPoint& HexagonCenter, TArray<FIntPoint>& OutVertices);

	UFUNCTION(BlueprintCallable)
	static void GetHexagonEdges(const FIntPoint& HexagonCenter, TArray<FIntPoint>& OutEdges);

	UFUNCTION(BlueprintCallable)
	static void GetHexagonNeighbours(const FIntPoint& Hexagon, TArray<FIntPoint>& OutNeighbours);

	UFUNCTION(BlueprintCallable)
	static void GetVertexHexagons(const FIntPoint& Vertex, TArray<FIntPoint>& OutHexagons);

	UFUNCTION(BlueprintCallable)
	static void GetVertexEdges(const FIntPoint& Vertex, TArray<FIntPoint>& OutEdges);

	UFUNCTION(BlueprintCallable)
	static void GetEdgeHexagons(const FIntPoint& Edge, TArray<FIntPoint>& OutHexagons);

	UFUNCTION(BlueprintCallable)
	static void GetEdgeVertices(const FIntPoint& Edge, TArray<FIntPoint>& OutVertices);

	UFUNCTION(BlueprintCallable)
	static void GetEdgeEdges(const FIntPoint& Edge, TArray<FIntPoint>& OutEdges);

	UFUNCTION(BlueprintCallable)
	static FVector CalculateGridBottomLeft(FVector Center, FVector2D GridSize, FVector TileSize);

	UFUNCTION(BlueprintCallable)
	static FIntPoint MakeTileIndex(int32 X, int32 Y);

	UFUNCTION(BlueprintCallable)
	static FVector TileIndexToWorld(const FIntPoint& TileIndex, const FVector& GridBottomLeft, const FVector& TileSize);

	UFUNCTION(BlueprintCallable)
	static FHexHitResult GetSnapResult(const FVector& Intersection, const FVector& GridBottomLeft, const FVector2D& TileDiv);

	UFUNCTION(BlueprintCallable)
	static void SpawnVerticesAndEdges(UWorld* World, AHexTiles* ParentTile, const FIntPoint& TileIndex, const FVector& GridBottomLeft, const FVector& TileSize, const FVector& SettlementScale, TMap<FIntPoint, AHexTiles*>& OutSettlements, TMap<FIntPoint, AHexTiles*>& OutRoads, const UHexBoardConfig* Config);

	static void ForEachValidHexRow(int32 Column, FIntPoint GridSize, const TFunctionRef<void(int32 Row)>& Func);

	UFUNCTION(BlueprintCallable)
	static bool IsValidEndTurn(const FHexHitResult& InSelection, AHexGameState* HexGs, AHexPlayerState* HexPs);

	UFUNCTION(BlueprintCallable)
	static bool TraceMouseToGrid(const APlayerController* PlayerController, const FVector& GridCenterLocation, FVector& OutIntersection);
};
