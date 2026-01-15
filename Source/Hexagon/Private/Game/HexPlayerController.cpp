// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/HexPlayerController.h"
#include "Game/HexPlayerState.h"
#include "Game/HexGameState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HexGridGenerator.h"
#include "HexHelper.h"
#include "HexMath.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AHexPlayerController::AHexPlayerController()
{
	bShowMouseCursor = true;
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

	if(IsLocalController() && HoverWidgetClass)
	{
		HoverWidget = CreateWidget(this, HoverWidgetClass);
		HoverWidget->AddToViewport();
		HoverWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AHexPlayerController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateHoverWidget();
}

void AHexPlayerController::UpdateHoverWidget()
{
	if(!HoverWidget || !AHexGridGen || !IsLocalController()) return;

	if(SendCurrentHoverSelection.SnapType == EHexSnapType::None)
	{
		HoverWidget->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	HoverWidget->SetVisibility(ESlateVisibility::Visible);

	float X, Y;
	GetMousePosition(X, Y);
	HoverWidget->SetPositionInViewport(FVector2D(X + 12.f, Y + 12.f));
}

void AHexPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &AHexPlayerController::OnLeftMouseClicked);
		EnhancedInputComponent->BindAction(EndTurnAction, ETriggerEvent::Started, this, &AHexPlayerController::ActionEndTurn);
	}
}

void AHexPlayerController::ActionEndTurn(const FInputActionValue& Value)
{
	EndTurn();
}

void AHexPlayerController::EndTurn()
{
	Server_EndTurn(SendCurrentHoverSelection);

	AHexPlayerState* PS = GetPlayerState<AHexPlayerState>();
	AHexGameState* GS = GetWorld()->GetGameState<AHexGameState>();

	if(UHexMath::IsValidEndTurn(SendCurrentHoverSelection, GS, PS)) return;

	// TODO: create a widget for error messages to player when end turn is pressed.
}

void AHexPlayerController::OnLeftMouseClicked(const FInputActionValue& Value)
{
	if(!AHexGridGen)
		AHexGridGen = Cast<AHexGridGenerator>(UGameplayStatics::GetActorOfClass(this, AHexGridGenerator::StaticClass()));

	if(!AHexGridGen) return;

	SendCurrentHoverSelection = AHexGridGen->SendCurrentHoverSelection();
}

void AHexPlayerController::Server_EndTurn_Implementation(const FHexHitResult& InSelection)
{
	AHexPlayerState* PS = GetPlayerState<AHexPlayerState>();
	AHexGameState* GS = GetWorld()->GetGameState<AHexGameState>();

	const FString Output = UHexHelper::ExportStructToText(InSelection);

	if(!UHexMath::IsValidEndTurn(InSelection, GS, PS))
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Not Valid turn change by: %s with data: %s"), *PS->GetPlayerName(), *Output));
		return;
	}

	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("turn change: %s"), *Output));
	GS->AdvanceTurn();
}
