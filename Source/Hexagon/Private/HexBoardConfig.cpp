// Fill out your copyright notice in the Description page of Project Settings.


#include "HexBoardConfig.h"

FShuffledTiles UHexBoardConfig::GetShuffledTiles(const int32 Seed) const
{
	TArray<EHexTileType> TileTypes;
	for(const auto& Pair : TileTypeDistribution)
	{
		for (int32 i = 0; i < Pair.Value; ++i)
		{
			TileTypes.Add(Pair.Key);
		}
	}

	// Standard Catan dice number distribution (excluding desert)
	TArray<int32> DiceNumbers = {
		2,
		3, 3,
		4, 4,
		5, 5,
		6, 6,
		8, 8,
		9, 9,
		10, 10,
		11, 11,
		12
	};

	ShuffleArray(TileTypes, Seed);
	ShuffleArray(DiceNumbers, Seed + 1);

	FShuffledTiles ShuffledTiles;
	ShuffledTiles.ShuffledTileTypes = TileTypes;

	ShuffledTiles.ShuffledDiceNumbers.Reserve(TileTypes.Num());

	int32 DiceIndex = 0;

	for (const EHexTileType TileType : TileTypes)
	{
		if (TileType == EHexTileType::Desert)
			ShuffledTiles.ShuffledDiceNumbers.Add(-1); // 0 = no dice
		else
		{
			ShuffledTiles.ShuffledDiceNumbers.Add(DiceNumbers[DiceIndex]);
			++DiceIndex;
		}
	}

	return ShuffledTiles;
}
