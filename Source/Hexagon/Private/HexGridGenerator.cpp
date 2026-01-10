// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGridGenerator.h"
#include "HexBoardConfig.h"
#include "HexMath.h"
#include "HexTiles.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


AHexGridGenerator::AHexGridGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

void AHexGridGenerator::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	InitializeGrid();

	// Server Generates the seed for the game
	if(!HasAuthority()) return;

	GridGenSeed = FMath::Rand32();
	GenerateBoardTiles(GridGenSeed);
}

void AHexGridGenerator::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Intersection;
	if (!TraceMouseToGrid(Intersection)) return;

	FVector2D MousePos;
	if (!PlayerController->GetMousePosition(MousePos.X, MousePos.Y)) return;

	if (MousePos.Equals(LastMousePos, 0.1f)) return;
	LastMousePos = MousePos;

	UpdateHoveredTile(Intersection);
}

void AHexGridGenerator::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, GridGenSeed);
}

void AHexGridGenerator::InitializeGrid()
{
	const FVector& TileSize = BoardConfig->GridTileSize;

	SnapGrid = UHexMath::CalculateSnapGrid(TileSize);
	TileDiv = UHexMath::CalculateTileDiv(TileSize);

	GridCenterLocation = UHexMath::SnapVectorToVector(GetActorLocation(), TileSize);
	GridBottomLeftLocation = UHexMath::CalculateGridBottomLeft(GridCenterLocation, BoardConfig->GetGridTileCount(), TileSize);
}

void AHexGridGenerator::OnRep_GridGenSeed()
{
	GenerateBoardTiles(GridGenSeed);
}

void AHexGridGenerator::GenerateBoardTiles(const int32 Seed)
{
	const FIntPoint GridSize = BoardConfig->GetGridTileCount().IntPoint();
	const FVector& TileSize = BoardConfig->GridTileSize;

	const FVector TileScale = BoardConfig->TileMeshSize / TileSize;
	const FVector SettlementScale = BoardConfig->SettlementMeshSize / TileSize;

	const TArray<EHexTileType>& TileTypes = BoardConfig->GetShuffledTileTypes(Seed);
	int32 TileTypeIndex = 0;

	for(int32 X = 0; X < GridSize.X; ++X)
	{
		UHexMath::ForEachValidHexRow(X, GridSize, [&](const int32 Y)
			{
				const FIntPoint TileIndex = UHexMath::MakeTileIndex(X, Y);
				const FVector TileLocation = UHexMath::TileIndexToWorld(TileIndex, GridBottomLeftLocation, TileSize);

				AHexTiles*& Tile = PlacedTiles.FindOrAdd(TileIndex);
				if(!Tile)
				{
					const FTransform Transform(FRotator::ZeroRotator, TileLocation, TileScale);
					Tile = GetWorld()->SpawnActorDeferred<AHexTiles>(BoardConfig->HexTileClass, Transform);

					Tile->TileCoordinates = TileIndex;
					Tile->TileType = TileTypes[TileTypeIndex++ % TileTypes.Num()];
					Tile->TileColor = BoardConfig->TileTypeColor[Tile->TileType];
					UGameplayStatics::FinishSpawningActor(Tile, Transform, ESpawnActorScaleMethod::OverrideRootScale);
				}

				UHexMath::SpawnVerticesAndEdges(GetWorld(), Tile, TileIndex, GridBottomLeftLocation, TileSize, SettlementScale, PlacedSettlements, PlacedRoads, BoardConfig);
			});
	}
}

bool AHexGridGenerator::TraceMouseToGrid(FVector& OutIntersection) const
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

void AHexGridGenerator::UpdateHoveredTile(const FVector& Intersection)
{
	const auto [bHit, SnapType, ClosestIndex] = UHexMath::GetSnapResult(Intersection, GridBottomLeftLocation, TileDiv);

	if (!bHit) return;

	if (TileUnderMouseInfo.Index == ClosestIndex) return;

	if (TileUnderMouseInfo.Actor)
	{
		TileUnderMouseInfo.Actor->OnUnHighlight();
	}

	auto& Map = SnapType == EHexSnapType::Tile ? PlacedTiles : (SnapType == EHexSnapType::Vertex ? PlacedSettlements : PlacedRoads);;

	if (AHexTiles** Found = Map.Find(ClosestIndex))
	{
		TileUnderMouseInfo.Actor = *Found;
		TileUnderMouseInfo.Index = ClosestIndex;
		TileUnderMouseInfo.Actor->OnHighlight();
	}
}
