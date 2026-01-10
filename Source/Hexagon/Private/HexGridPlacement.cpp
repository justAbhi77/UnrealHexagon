// Fill out your copyright notice in the Description page of Project Settings.

#include "HexGridPlacement.h"
#include "HexBoardConfig.h"
#include "HexTiles.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "HexMath.h"

AHexGridPlacement::AHexGridPlacement()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root"));
}

void AHexGridPlacement::BeginPlay()
{
	Super::BeginPlay();

	const FVector& GridTileSize = BoardConfig->GridTileSize;

	SnapGrid = FVector(GridTileSize.X * 0.125f, GridTileSize.Y * 0.25f * One6, 1.f);
	TileDiv = FVector2D(GridTileSize.X * 0.125f, GridTileSize.Y);

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	GenerateBoardTiles(GetActorLocation(), GridTileSize);
}

void AHexGridPlacement::GenerateBoardTiles(const FVector& Center, const FVector& TileSize)
{
	// Cache grid dimensions
	const FVector2D& GridTileCount = BoardConfig->GetGridTileCount();
	GridCenterLocation = UHexMath::SnapVectorToVector(Center, TileSize);

	const FVector2D GridOffset = GridTileCount / FVector2D(3, 3);
	const FVector SnappedTileCount = TileSize * FVector(GridOffset.X, GridOffset.Y, 0.f);
	GridBottomLeftLocation = GridCenterLocation - UHexMath::SnapVectorToVector(SnappedTileCount, TileSize);

	const float TileX075 = TileSize.X * 0.125f;
	const float TileY05  = TileSize.Y * 0.5f * One6;
	const FVector& TileScale = BoardConfig->TileMeshSize / BoardConfig->GridTileSize;
	const FVector& SettlementScale = BoardConfig->SettlementMeshSize / BoardConfig->GridTileSize;

	// Determine grid parity
	const bool bShouldNotSubtract = static_cast<int>((GridTileCount.X + 1) / 2) % 2 == 0;
	const auto& TileTypes = BoardConfig->GetShuffledTileTypes(0);
	int32 TileTypeIndex = 0;

	for(int32 x = 0; x < GridTileCount.X; x++)
	{
		const int Bounds = FMath::Abs((GridTileCount.X - (x * 2 + 1)) / 2);
		const int LowerBound = bShouldNotSubtract ? Bounds + 1 : Bounds;
		const int RowOffset = Bounds % 2 == 0 ? Bounds : Bounds - 1;
		const int EndY = GridTileCount.Y * 2 - 1 - RowOffset;
		const int UpperBound = bShouldNotSubtract ? EndY : EndY - 1;

		for(int32 y = LowerBound; y <= UpperBound; y += 2)
		{
			const FIntPoint TileIndex(x * 6, y * 6);
			const FVector TileLocation = GridBottomLeftLocation + FVector(TileIndex.X * TileX075, TileIndex.Y * TileY05, 0.f);

			// Spawn tile if missing
			AHexTiles*& NewTile = PlacedTiles.FindOrAdd(TileIndex, nullptr);
			if(!NewTile)
			{
				FTransform SpawnTransform(FRotator::ZeroRotator, TileLocation, TileScale);
				NewTile = GetWorld()->SpawnActorDeferred<AHexTiles>(BoardConfig->HexTileClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, ESpawnActorScaleMethod::OverrideRootScale);
				NewTile->TileCoordinates = TileIndex;

				// border tiles
				//if(x == 0 || y == LowerBound || x == GridTileCount.X - 1 || y == UpperBound || y == UpperBound - 1)
				NewTile->TileType = TileTypes[TileTypeIndex];
				NewTile->TileColor = BoardConfig->TileTypeColor[TileTypes[TileTypeIndex]];
				TileTypeIndex++;
				TileTypeIndex %= TileTypes.Num();

				UGameplayStatics::FinishSpawningActor(NewTile, SpawnTransform, ESpawnActorScaleMethod::OverrideRootScale);
			}

			// Process vertices
			UHexMath::GetHexagonVertices(TileIndex, HexVerticesCache);

			for(int32 i = 0; i < HexVerticesCache.Num(); i++)
			{
				FIntPoint Vertex = HexVerticesCache[i];
				const FVector VertexWorld = GridBottomLeftLocation + FVector(Vertex.X * TileX075, Vertex.Y * TileY05, TileSize.Z * 1.1f);
				// const FVector VertexWorld = GridBottomLeftLocation + FVector(Vertex, 1.1f) * TileSize;

				// Spawn settlement
				AHexTiles*& Settlement = PlacedSettlements.FindOrAdd(Vertex, nullptr);
				if(!Settlement)
				{
					FTransform SpawnTransform(FRotator::ZeroRotator, VertexWorld, SettlementScale);
					Settlement = GetWorld()->SpawnActorDeferred<AHexTiles>(BoardConfig->SettlementTileClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, ESpawnActorScaleMethod::OverrideRootScale);
					Settlement->TileCoordinates = Vertex;
					UGameplayStatics::FinishSpawningActor(Settlement, SpawnTransform, ESpawnActorScaleMethod::OverrideRootScale);

					Settlement->AttachToActor(NewTile, FAttachmentTransformRules::KeepWorldTransform);
				}

				// Spawn road
				const int NextIndex = (i + 1) % HexVerticesCache.Num();
				// const FVector NextWorld = GridBottomLeftLocation + FVector(HexVerticesCache[NextIndex], 1.1f) * TileSize;
				const FVector NextWorld = GridBottomLeftLocation + FVector(HexVerticesCache[NextIndex].X * TileX075, HexVerticesCache[NextIndex].Y * TileY05, TileSize.Z * 1.1f);
				const FIntPoint MidPoint = (Vertex + HexVerticesCache[NextIndex]) / 2.f;
				const FVector MidPointWorld = (VertexWorld + NextWorld) / 2.f;

				AHexTiles*& Road = PlacedRoads.FindOrAdd(MidPoint, nullptr);
				if(Road) continue;

				const float RoadLength = (VertexWorld - NextWorld).Size();
				const FRotator Rotation = UKismetMathLibrary::MakeRotFromX(NextWorld - VertexWorld);

				FTransform SpawnTransform(Rotation, MidPointWorld, FVector(RoadLength, BoardConfig->RoadMeshSizeY, 1.f));
				Road = GetWorld()->SpawnActorDeferred<AHexTiles>(BoardConfig->RoadTileClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, ESpawnActorScaleMethod::OverrideRootScale);
				Road->TileCoordinates = MidPoint;
				UGameplayStatics::FinishSpawningActor(Road, SpawnTransform, ESpawnActorScaleMethod::OverrideRootScale);
			}
		}
	}
}

FHexHitResult AHexGridPlacement::GetTileUnderMouse(const FVector& Intersection)
{
	FHexHitResult Result;

	const FVector GridLocal = Intersection - GridBottomLeftLocation;
	const FVector GridSpacePos = GridLocal * FVector(1.f, 2.f, 1.f);

	const FVector2D TileIndexContinuous = FVector2D(GridSpacePos) / TileDiv;
	const int32 BucketX = FMath::DivideAndRoundDown(TileIndexContinuous.X + 2.3, 6.0);

	FVector2D TileIndexSnapped;
	FVector2D TileIndexUnSnapped;

	TileIndexSnapped.X = BucketX * 6;
	TileIndexUnSnapped.X = TileIndexContinuous.X + 2.3f / 6.f;
	TileIndexUnSnapped.Y = (TileIndexContinuous.Y / 2.f) * 12.f;
	if(BucketX % 2 == 0)
	{
		TileIndexSnapped.Y = FMath::RoundToInt(TileIndexContinuous.Y / 2.f) * 12.f;
	} else
	{
		TileIndexSnapped.Y = FMath::FloorToInt(TileIndexContinuous.Y / 2.f) * 12.f + 6.f;
	}

	float ClosestDist = FVector2D::DistSquared(TileIndexUnSnapped, TileIndexSnapped);
	Result.SnapType = EHexSnapType::Tile;
	Result.ClosestIndex = FIntPoint(TileIndexSnapped.X, TileIndexSnapped.Y);

	UHexMath::GetHexagonVertices(Result.ClosestIndex, HexVerticesCache);
	UHexMath::GetHexagonEdges(Result.ClosestIndex, HexEdgesCache);

	for(int32 i = 0; i < HexVerticesCache.Num(); i++)
	{
		const float IndexSnappedVertexDis = FVector2D::DistSquared(TileIndexUnSnapped, HexVerticesCache[i]);
		const float IndexSnappedEdgeDis = FVector2D::DistSquared(TileIndexUnSnapped, HexEdgesCache[i]);

		if(IndexSnappedVertexDis < ClosestDist)
		{
			ClosestDist = IndexSnappedVertexDis;
			Result.SnapType = EHexSnapType::Vertex;
			Result.ClosestIndex = HexVerticesCache[i];
		}
		if(IndexSnappedEdgeDis < ClosestDist)
		{
			ClosestDist = IndexSnappedEdgeDis;
			Result.SnapType = EHexSnapType::Edge;
			Result.ClosestIndex = HexEdgesCache[i];
		}
	}

	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 99.f, FColor::Green, FString::Printf(TEXT("Mouse Location: %s"), *TileIndexUnSnapped.ToString()));
		GEngine->AddOnScreenDebugMessage(1, 99.f, FColor::Green, FString::Printf(TEXT("Tile Location: %s"), *TileIndexSnapped.ToString()));
		GEngine->AddOnScreenDebugMessage(2, 99.f, FColor::Green, FString::Printf(TEXT("Closest Location: %s"), *Result.ClosestIndex.ToString()));
	}

	Result.bHit = true;
	return Result;
}

void AHexGridPlacement::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!PlayerController) return;

	FVector WorldLocation; FVector WorldDirection;
	PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	const FVector End = WorldLocation + WorldDirection * 100000.f;
	const FPlane GridPlane = UKismetMathLibrary::MakePlaneFromPointAndNormal(GridCenterLocation, FVector::UpVector);

	FVector Intersection;
	if(float T; !UKismetMathLibrary::LinePlaneIntersection(WorldLocation, End, GridPlane, T, Intersection)) return;

	if(const auto [bHit, SnapType, ClosestIndex] = GetTileUnderMouse(Intersection); bHit)
	{
		FVector2D MousePos;
		if(!PlayerController->GetMousePosition(MousePos.X, MousePos.Y)) return;

		if(MousePos.Equals(LastMousePos, 0.1f)) return; // no significant movement
		LastMousePos = MousePos;

		if(TileUnderMouseInfo.Index != ClosestIndex)
		{
			TileUnderMouseInfo.Index = ClosestIndex;

			if(TileUnderMouseInfo.Actor) TileUnderMouseInfo.Actor->OnUnHighlight();

			auto& PlacedMap = SnapType == EHexSnapType::Tile ? PlacedTiles : (SnapType == EHexSnapType::Vertex ? PlacedSettlements : PlacedRoads);
			if(AHexTiles** ExistingTile = PlacedMap.Find(ClosestIndex))
			{
				TileUnderMouseInfo.Actor = *ExistingTile;
				if(TileUnderMouseInfo.Actor) TileUnderMouseInfo.Actor->OnHighlight();
			}
		}
	}
	else
	{
		if(TileUnderMouseInfo.Actor)
		{
			TileUnderMouseInfo.Actor->OnUnHighlight();
			TileUnderMouseInfo.Actor = nullptr;
		}
	}
}
