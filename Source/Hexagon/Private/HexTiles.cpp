// Fill out your copyright notice in the Description page of Project Settings.


#include "HexTiles.h"


AHexTiles::AHexTiles()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);
}

void AHexTiles::OnHighlight()
{
	OnTileHighlighted();
}

void AHexTiles::OnUnHighlight()
{
	OnTileUnHighlighted();
}

void AHexTiles::BeginPlay()
{
	Super::BeginPlay();

}

void AHexTiles::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

