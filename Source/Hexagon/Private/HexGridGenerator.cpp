// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGridGenerator.h"
#include "HexBoardConfig.h"
#include "HexMath.h"
#include "HexTiles.h"
#include "Game/HexPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


AHexGridGenerator::AHexGridGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

void AHexGridGenerator::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController = Cast<AHexPlayerController>(Controller);

	InitializeGrid();

	// Server Generates the seed for the game
	if(!HasAuthority()) return;

	GridGenSeed = FMath::Rand32();
	StartGenerateBoard(GridGenSeed);
}

void AHexGridGenerator::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Intersection;
	if (!UHexMath::TraceMouseToGrid(PlayerController, GridCenterLocation, Intersection)) return;

	FVector2D MousePos;
	if(!PlayerController->GetMousePosition(MousePos.X, MousePos.Y)) return;

	if(MousePos.Equals(LastMousePos, 0.1f)) return;
	LastMousePos = MousePos;

	UpdateHoveredTile(Intersection);
}

void AHexGridGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
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
	StartGenerateBoard(GridGenSeed);
}

void AHexGridGenerator::UpdateHoveredTile(const FVector& Intersection)
{
	MouseHoverResult = UHexMath::GetSnapResult(Intersection, GridBottomLeftLocation, TileDiv);

	if (TileUnderMouseInfo.Index == MouseHoverResult.ClosestIndex) return;

	if (TileUnderMouseInfo.Actor)
	{
		TileUnderMouseInfo.Actor->OnUnHighlight();
	}

	if (AHexTiles** Found = SpawnedHexTiles.Find(MouseHoverResult.ClosestIndex))
	{
		MouseHoverResult.bHit = true;
		TileUnderMouseInfo.Actor = *Found;
		TileUnderMouseInfo.Index = MouseHoverResult.ClosestIndex;
		TileUnderMouseInfo.Actor->OnHighlight();
	}
}

void AHexGridGenerator::StartGenerateBoard(const int32 Seed)
{
	CurrentX = 0;
	CurrentColumnRows.Reset();

	GridSize = BoardConfig->GetGridTileCount();
	const auto [ShuffledTileTypes, ShuffledDiceNumbers] = BoardConfig->GetShuffledTiles(Seed);
	TileTypes = ShuffledTileTypes;
	TileDiceNumbers = ShuffledDiceNumbers;
	TileTypeIndex = 0;

	GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AHexGridGenerator::SpawnNextHex, 0.05f, true);
}

void AHexGridGenerator::SpawnNextHex()
{
	// Move to next column if needed
	if (CurrentColumnRows.IsEmpty())
	{
		if (CurrentX >= GridSize.X)
		{
			GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
			return;
		}

		UHexMath::ForEachValidHexRow(CurrentX, GridSize, [this](const int32 Y)
		{
			CurrentColumnRows.Add(Y);
		});

		CurrentYIndex = 0;
	}

	const int32 Y = CurrentColumnRows[CurrentYIndex++];
	const FIntPoint TileIndex = UHexMath::MakeTileIndex(CurrentX, Y);

	SpawnSingleHex(TileIndex);

	if (CurrentYIndex >= CurrentColumnRows.Num())
	{
		CurrentColumnRows.Reset();
		++CurrentX;
	}
}

void AHexGridGenerator::SpawnSingleHex(const FIntPoint& TileIndex)
{
	const FVector& TileSize = BoardConfig->GridTileSize;
	const FVector TileScale = BoardConfig->TileMeshSize / TileSize;
	const FVector SettlementScale = BoardConfig->SettlementMeshSize / TileSize;

	const FVector TileLocation = UHexMath::TileIndexToWorld(TileIndex, GridBottomLeftLocation, TileSize);

	AHexTiles*& Tile = SpawnedHexTiles.FindOrAdd(TileIndex);
	if(!Tile)
	{
		const FTransform Transform(FRotator::ZeroRotator, TileLocation, TileScale);
		Tile = GetWorld()->SpawnActorDeferred<AHexTiles>(BoardConfig->HexTileClass, Transform);

		Tile->TileCoordinates = TileIndex;
		Tile->TileType = TileTypes[TileTypeIndex % TileTypes.Num()];
		Tile->DiceNumber = TileDiceNumbers[TileTypeIndex];
		Tile->TileColor = BoardConfig->TileTypeColor[Tile->TileType];

		UGameplayStatics::FinishSpawningActor(Tile, Transform, ESpawnActorScaleMethod::OverrideRootScale);
		TileTypeIndex++;
	}

	UHexMath::SpawnVerticesAndEdges(GetWorld(), Tile, TileIndex, GridBottomLeftLocation, TileSize, SettlementScale, SpawnedHexTiles, BoardConfig);
}

FHexHitResult AHexGridGenerator::SendCurrentHoverSelection() const
{
	return MouseHoverResult;
}
