// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/HexPlayerController.h"
#include "Game/HexPlayerState.h"
#include "Game/HexGameState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HexGridGenerator.h"
#include "Kismet/GameplayStatics.h"

AHexPlayerController::AHexPlayerController()
{
	bShowMouseCursor = true;
}

void AHexPlayerController::SendSelection(const FHexHitResult& Selection)
{
	Server_SendSelection(Selection);
}

void AHexPlayerController::EndTurn(const FInputActionValue& Value)
{
	Server_EndTurn();
}

void AHexPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &AHexPlayerController::OnLeftMouseClicked);
		EnhancedInputComponent->BindAction(EndTurnAction, ETriggerEvent::Started, this, &AHexPlayerController::EndTurn);
	}
}

void AHexPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void AHexPlayerController::OnLeftMouseClicked(const FInputActionValue& Value)
{
	if(!AHexGridGen)
	{
		AHexGridGen = Cast<AHexGridGenerator>(UGameplayStatics::GetActorOfClass(this, AHexGridGenerator::StaticClass()));
	}

	if(!AHexGridGen) return;

	const FHexHitResult SendCurrentHoverSelection = AHexGridGen->SendCurrentHoverSelection();

	SendSelection(SendCurrentHoverSelection);
}

void AHexPlayerController::Server_SendSelection_Implementation(const FHexHitResult& Selection)
{
	AHexPlayerState* PS = GetPlayerState<AHexPlayerState>();
	const AHexGameState* GS = GetWorld()->GetGameState<AHexGameState>();

	if(!PS || !GS) return;

	if(GS->GetActivePlayer() != PS)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("non Selection is %s"), *Selection.ClosestIndex.ToString()));
		return;
	}

	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Selection is %s"), *Selection.ClosestIndex.ToString()));

	PS->LastSelection = Selection;
}

void AHexPlayerController::Server_EndTurn_Implementation()
{
	const AHexPlayerState* PS = GetPlayerState<AHexPlayerState>();
	AHexGameState* GS = GetWorld()->GetGameState<AHexGameState>();

	if(!PS || !GS) return;

	if(GS->GetActivePlayer() != PS) return;

	GS->AdvanceTurn();
}
