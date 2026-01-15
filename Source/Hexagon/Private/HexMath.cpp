// Fill out your copyright notice in the Description page of Project Settings.


#include "HexMath.h"
#include "HexTiles.h"
#include "HexTypes.h"
#include "HexBoardConfig.h"
#include "Game/HexGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

FVector UHexMath::CalculateSnapGrid(const FVector& TileSize)
{
	return FVector(TileSize.X * 0.125f, TileSize.Y * 0.25f * One6, 1.f);
}

FVector2D UHexMath::CalculateTileDiv(const FVector& TileSize)
{
	return FVector2D(TileSize.X * 0.125f, TileSize.Y);
}

FVector UHexMath::SnapVectorToVector(const FVector& VectorToSnap, const FVector& GridVector)
{
	const float ResultX = FMath::GridSnap(VectorToSnap.X, GridVector.X);
	const float ResultY = FMath::GridSnap(VectorToSnap.Y, GridVector.Y);
	const float ResultZ = FMath::GridSnap(VectorToSnap.Z, GridVector.Z);

	return FVector(ResultX, ResultY, ResultZ);
}

void UHexMath::GetHexagonVertices(const FIntPoint& HexagonCenter, TArray<FIntPoint>& OutVertices)
{
	OutVertices.Reset(6);
	OutVertices.AddUninitialized(6);
	OutVertices[0] = HexagonCenter + FIntPoint(4, 0.f);
	OutVertices[1] = HexagonCenter + FIntPoint(2, 6.f);
	OutVertices[2] = HexagonCenter + FIntPoint(-2, 6.f);
	OutVertices[3] = HexagonCenter + FIntPoint(-4, 0.f);
	OutVertices[4] = HexagonCenter + FIntPoint(-2, -6.f);
	OutVertices[5] = HexagonCenter + FIntPoint(2, -6.f);
}

void UHexMath::GetHexagonEdges(const FIntPoint& HexagonCenter, TArray<FIntPoint>& OutEdges)
{
	OutEdges.Reset(6);
	OutEdges.AddUninitialized(6);
	OutEdges[0] = HexagonCenter + FIntPoint(3, 3.f);
	OutEdges[1] = HexagonCenter + FIntPoint(0, 6.f);
	OutEdges[2] = HexagonCenter + FIntPoint(-3.f, 3.f);
	OutEdges[3] = HexagonCenter + FIntPoint(-3.f, -3.f);
	OutEdges[4] = HexagonCenter + FIntPoint(0, -6.f);
	OutEdges[5] = HexagonCenter + FIntPoint(3.f, -3.f);
}

void UHexMath::GetHexagonNeighbours(const FIntPoint& Hexagon, TArray<FIntPoint>& OutNeighbours)
{
	OutNeighbours.Reset(6);
	OutNeighbours.AddUninitialized(6);
	OutNeighbours[0] = Hexagon + FIntPoint(6.f, -6.f);
	OutNeighbours[1] = Hexagon + FIntPoint(6.f, 6.f);
	OutNeighbours[2] = Hexagon + FIntPoint(0.f, 12.f);
	OutNeighbours[3] = Hexagon + FIntPoint(-6.f, 6.f);
	OutNeighbours[4] = Hexagon + FIntPoint(-6.f, -6.f);
	OutNeighbours[5] = Hexagon + FIntPoint(0.f, -12.f);
}

void UHexMath::GetVertexHexagons(const FIntPoint& Vertex, TArray<FIntPoint>& OutHexagons)
{
	OutHexagons.Reset(6);
	OutHexagons.AddUninitialized(6);

	// for pointy top vertices
	OutHexagons[0] = Vertex + FIntPoint(2, -6);
	OutHexagons[1] = Vertex + FIntPoint( 2, 6);
	OutHexagons[2] = Vertex + FIntPoint( -4,  0);

	// for the four adjacent vertices
	OutHexagons[3] = Vertex + FIntPoint(4, 0);
	OutHexagons[4] = Vertex + FIntPoint( -2, -6);
	OutHexagons[5] = Vertex + FIntPoint( -2,  6);
}

void UHexMath::GetVertexEdges(const FIntPoint& Vertex, TArray<FIntPoint>& OutEdges)
{
	OutEdges.Reset(6);
	OutEdges.AddUninitialized(6);

	// for pointy top vertices
	OutEdges[0] = Vertex + FIntPoint(2, 0);
	OutEdges[1] = Vertex + FIntPoint( -1, -3);
	OutEdges[2] = Vertex + FIntPoint( -1,  3);

	// for the four adjacent vertices
	OutEdges[3] = Vertex + FIntPoint(-2, 0);
	OutEdges[4] = Vertex + FIntPoint( 1, 3);
	OutEdges[5] = Vertex + FIntPoint( 1,  -3);
}

void UHexMath::GetEdgeHexagons(const FIntPoint& Edge, TArray<FIntPoint>& OutHexagons)
{
	OutHexagons.Reset(6);
	OutHexagons.AddUninitialized(6);

	// for pointy top right side
	OutHexagons[0] = Edge + FIntPoint(3, 3);
	OutHexagons[1] = Edge + FIntPoint( -3, -3);

	// for pointy top center same horizontal line
	OutHexagons[2] = Edge + FIntPoint(0, -6);
	OutHexagons[3] = Edge + FIntPoint( 0, 6);

	// for pointy top right side
	OutHexagons[4] = Edge + FIntPoint(3, -3);
	OutHexagons[5] = Edge + FIntPoint( -3, 3);
}

void UHexMath::GetEdgeVertices(const FIntPoint& Edge, TArray<FIntPoint>& OutVertices)
{
	OutVertices.Reset(6);
	OutVertices.AddUninitialized(6);

	// for pointy top right side
	OutVertices[0] = Edge + FIntPoint(1, -3);
	OutVertices[1] = Edge + FIntPoint( -1, 3);

	// for pointy top center same horizontal line
	OutVertices[2] = Edge + FIntPoint(2, 0);
	OutVertices[3] = Edge + FIntPoint( -2, 0);

	// for pointy top right side
	OutVertices[4] = Edge + FIntPoint(1, 3);
	OutVertices[5] = Edge + FIntPoint( -1, -3);
}

void UHexMath::GetEdgeEdges(const FIntPoint& Edge, TArray<FIntPoint>& OutEdges)
{
	OutEdges.Reset(12);
	OutEdges.AddUninitialized(12);

	// for pointy top right side
	OutEdges[0] = Edge + FIntPoint(3, -3);
	OutEdges[1] = Edge + FIntPoint( 0, -6);
	OutEdges[2] = Edge + FIntPoint(0, 6);
	OutEdges[3] = Edge + FIntPoint( -3, 3);

	// for pointy top center same horizontal line
	OutEdges[4] = Edge + FIntPoint(3, -3);
	OutEdges[5] = Edge + FIntPoint( 3, 3);
	OutEdges[6] = Edge + FIntPoint(-3, -3);
	OutEdges[7] = Edge + FIntPoint( -3, 3);

	// for pointy top right side
	OutEdges[8] = Edge + FIntPoint(3, 3);
	OutEdges[9] = Edge + FIntPoint( 0, -6);
	OutEdges[10] = Edge + FIntPoint(0, 6);
	OutEdges[11] = Edge + FIntPoint( -3, -3);
}

FVector UHexMath::CalculateGridBottomLeft(const FVector Center, const FVector2D GridSize, const FVector TileSize)
{
	// Your original offset logic
	const FVector2D GridOffset = GridSize / FVector2D(3.f, 3.f);
	const FVector SnappedTileCount = TileSize * FVector(GridOffset.X, GridOffset.Y, 0.f);

	return Center - SnapVectorToVector(SnappedTileCount, TileSize);
}

FIntPoint UHexMath::MakeTileIndex(int32 X, int32 Y)
{
	return FIntPoint(X * 6, Y * 6);
}

FVector UHexMath::TileIndexToWorld(const FIntPoint& TileIndex, const FVector& GridBottomLeft, const FVector& TileSize)
{
	const float TileX075 = TileSize.X * 0.125f;
	const float TileY05  = TileSize.Y * 0.5f * One6;

	return GridBottomLeft + FVector(TileIndex.X * TileX075, TileIndex.Y * TileY05, 0.f);
}

FHexHitResult UHexMath::GetSnapResult(const FVector& Intersection, const FVector& GridBottomLeft, const FVector2D& TileDiv)
{
	FHexHitResult Result;

	const FVector GridLocal = Intersection - GridBottomLeft;
	const FVector GridSpace = GridLocal * FVector(1.f, 2.f, 1.f);

	const FVector2D TileIndexContinuous = FVector2D(GridSpace) / TileDiv;

	FVector2D Unsnapped;
	Unsnapped.X = TileIndexContinuous.X + 2.3f / 6.f;
	Unsnapped.Y = (TileIndexContinuous.Y / 2.f) * 12.f;

	FVector2D Snapped;

	const int32 BucketX = FMath::DivideAndRoundDown(TileIndexContinuous.X + 2.3, 6.0);
	Snapped.X = BucketX * 6;

	if (BucketX % 2 == 0)
	{
		Snapped.Y = FMath::RoundToInt(TileIndexContinuous.Y / 2.f) * 12.f;
	}
	else
	{
		Snapped.Y = FMath::FloorToInt(TileIndexContinuous.Y / 2.f) * 12.f + 6.f;
	}

	float ClosestDist = FVector2D::DistSquared(Unsnapped, Snapped);

	Result.SnapType = EHexSnapType::Tile;
	Result.ClosestIndex = FIntPoint(Snapped.X, Snapped.Y);

	TArray<FIntPoint> Vertices;
	TArray<FIntPoint> Edges;

	GetHexagonVertices(Result.ClosestIndex, Vertices);
	GetHexagonEdges(Result.ClosestIndex, Edges);

	for (int32 i = 0; i < Vertices.Num(); ++i)
	{
		const float VDist = FVector2D::DistSquared(Unsnapped, Vertices[i]);
		if (VDist < ClosestDist)
		{
			ClosestDist = VDist;
			Result.SnapType = EHexSnapType::Vertex;
			Result.ClosestIndex = Vertices[i];
		}

		const float EDist = FVector2D::DistSquared(Unsnapped, Edges[i]);
		if (EDist < ClosestDist)
		{
			ClosestDist = EDist;
			Result.SnapType = EHexSnapType::Edge;
			Result.ClosestIndex = Edges[i];
		}
	}

	Result.bHit = true;
	return Result;
}

void UHexMath::ForEachValidHexRow(const int32 Column, const FIntPoint GridSize, const TFunctionRef<void(int32 Row)>& Func)
{
	const bool bShouldNotSubtract = ((GridSize.X + 1) / 2) % 2 == 0;

	const int32 Bounds = FMath::Abs((GridSize.X - (Column * 2 + 1)) / 2);

	const int32 LowerBound = bShouldNotSubtract ? Bounds + 1 : Bounds;

	const int32 RowOffset = Bounds % 2 == 0 ? Bounds : Bounds - 1;

	const int32 EndY = GridSize.Y * 2 - 1 - RowOffset;

	const int32 UpperBound = bShouldNotSubtract ? EndY : EndY - 1;

	for(int32 Y = LowerBound; Y <= UpperBound; Y += 2)
		Func(Y);
}

void UHexMath::SpawnVerticesAndEdges(UWorld* World, AHexTiles* ParentTile, const FIntPoint& TileIndex, const FVector& GridBottomLeft, const FVector& TileSize, const FVector& SettlementScale, TMap<FIntPoint, AHexTiles*>& OutSettlements, TMap<FIntPoint, AHexTiles*>& OutRoads, const UHexBoardConfig* Config)
{
	const float TileX075 = TileSize.X * 0.125f;
	const float TileY05  = TileSize.Y * 0.5f * One6;

	TArray<FIntPoint> Vertices;
	GetHexagonVertices(TileIndex, Vertices);

	for (int32 i = 0; i < Vertices.Num(); ++i)
	{
		const FIntPoint Vertex = Vertices[i];

		const FVector VertexWorld = GridBottomLeft + FVector(Vertex.X * TileX075, Vertex.Y * TileY05, TileSize.Z * 1.1f);

		/* ===== Settlement ===== */
		AHexTiles*& Settlement = OutSettlements.FindOrAdd(Vertex);
		if (!Settlement)
		{
			FTransform Transform(FRotator::ZeroRotator, VertexWorld, SettlementScale);

			Settlement = World->SpawnActorDeferred<AHexTiles>(Config->SettlementTileClass, Transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, ESpawnActorScaleMethod::OverrideRootScale);

			Settlement->TileCoordinates = Vertex;
			Settlement->TileType = EHexTileType::Settlement;

			UGameplayStatics::FinishSpawningActor(Settlement, Transform, ESpawnActorScaleMethod::OverrideRootScale);

			Settlement->AttachToActor(ParentTile, FAttachmentTransformRules::KeepWorldTransform);
		}

		/* ===== Road ===== */
		const int32 NextIndex = (i + 1) % Vertices.Num();
		const FIntPoint NextVertex = Vertices[NextIndex];

		const FVector NextWorld = GridBottomLeft + FVector(NextVertex.X * TileX075, NextVertex.Y * TileY05, TileSize.Z * 1.1f);

		const FIntPoint MidPoint = (Vertex + NextVertex) / 2;
		const FVector MidPointWorld = (VertexWorld + NextWorld) * 0.5f;

		AHexTiles*& Road = OutRoads.FindOrAdd(MidPoint);
		if (Road)
			continue;

		const float RoadLength = (VertexWorld - NextWorld).Size();
		const FRotator Rotation = UKismetMathLibrary::MakeRotFromX(NextWorld - VertexWorld);

		FTransform RoadTransform(Rotation, MidPointWorld, FVector(RoadLength, Config->RoadMeshSizeY, 1.f));

		Road = World->SpawnActorDeferred<AHexTiles>(Config->RoadTileClass, RoadTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, ESpawnActorScaleMethod::OverrideRootScale);

		Road->TileCoordinates = MidPoint;
		Road->TileType = EHexTileType::Road;

		UGameplayStatics::FinishSpawningActor(Road, RoadTransform, ESpawnActorScaleMethod::OverrideRootScale);
	}
}

bool UHexMath::IsValidEndTurn(const FHexHitResult& InSelection, AHexGameState* HexGs, AHexPlayerState* HexPs)
{
	if(!HexPs || !HexGs) return false;

	if(HexGs->GetActivePlayer() != HexPs) return false;

	switch(HexGs->TurnPhase)
	{
	case EHexTurnPhase::Setup:
		switch(HexGs->SetupRound)
		{
		case EHexSetupRound::First:
			if(InSelection.SnapType == EHexSnapType::Vertex)
				return true;
		case EHexSetupRound::Second:
			if(InSelection.SnapType == EHexSnapType::Edge)
				return true;
		}
		break;
	case EHexTurnPhase::Main:
		return true;
	}

	return false;
}

bool UHexMath::TraceMouseToGrid(const APlayerController* PlayerController, const FVector& GridCenterLocation, FVector& OutIntersection)
{
	if (!PlayerController)
		return false;

	FVector WorldLocation;
	FVector WorldDirection;

	if (!PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		return false;

	const FVector TraceEnd = WorldLocation + WorldDirection * 100000.f;

	// Grid plane (XY plane at grid center Z)
	const FPlane GridPlane = UKismetMathLibrary::MakePlaneFromPointAndNormal(GridCenterLocation, FVector::UpVector);

	if (float T; !UKismetMathLibrary::LinePlaneIntersection(WorldLocation, TraceEnd, GridPlane, T, OutIntersection))
		return false;

	return true;
}
