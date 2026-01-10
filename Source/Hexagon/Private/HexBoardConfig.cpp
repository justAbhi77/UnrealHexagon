// Fill out your copyright notice in the Description page of Project Settings.


#include "HexBoardConfig.h"

TArray<EHexTileType> UHexBoardConfig::GetShuffledTileTypes(const int32 Seed) const
{
	TArray<EHexTileType> TileTypes;
	for(const auto& Pair : TileTypeDistribution)
	{
		for (int32 i = 0; i < Pair.Value; ++i)
		{
			TileTypes.Add(Pair.Key);
		}
	}

	ShuffleArray(TileTypes, Seed);
	return TileTypes;
}
