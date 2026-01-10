// Fill out your copyright notice in the Description page of Project Settings.


#include "HexBoardConfig.h"
#include "Algo/RandomShuffle.h"

TArray<EHexTileType> UHexBoardConfig::GetShuffledTileTypes() const
{
	TArray<EHexTileType> TileTypes;
	for(const auto& Pair : TileTypeDistribution)
	{
		for (int32 i = 0; i < Pair.Value; ++i)
		{
			TileTypes.Add(Pair.Key);
		}
	}

	Algo::RandomShuffle(TileTypes);

	return TileTypes;
}
