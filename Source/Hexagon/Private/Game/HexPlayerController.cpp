// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/HexPlayerController.h"
#include "Game/HexPlayerState.h"
#include "Game/HexGameState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HexGridGenerator.h"
#include "HexHelper.h"
#include "HexMath.h"
#include "HexTiles.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Ui/HexDataUi.h"
#include "Ui/HexHoverUi.h"
#include "Ui/HexHudUi.h"

AHexPlayerController::AHexPlayerController()
{
	bShowMouseCursor = true;
}

void AHexPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(IsLocalController())
	{
		if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}

		if(HoverWidgetClass)
		{
			HoverWidget = CreateWidget<UHexHoverUi>(this, HoverWidgetClass);
			HoverWidget->AddToViewport();
			HoverWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		if(HudWidgetClass)
		{
			HudWidget = CreateWidget<UHexHudUi>(this, HudWidgetClass);
			HudWidget->AddToViewport();
			HudWidget->SetVisibility(ESlateVisibility::Visible);
			HudWidget->OnEndTurnPressed.AddDynamic(this, &AHexPlayerController::OnEndTurnPressed);
		}

		HexGameState = Cast<AHexGameState>(UGameplayStatics::GetGameState(GetWorld()));
		if (HexGameState)
		{
			HexGameState->OnTurnIndexChanged.AddUniqueDynamic(this, &AHexPlayerController::OnTurnIndexChanged);
			OnTurnIndexChanged();

			HexGameState->OnPhaseChanged.AddUniqueDynamic(this, &AHexPlayerController::OnPhaseChanged);
			OnPhaseChanged();
		}
	}

	if(!AHexGridGen)
		AHexGridGen = Cast<AHexGridGenerator>(UGameplayStatics::GetActorOfClass(this, AHexGridGenerator::StaticClass()));
}

void AHexPlayerController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!AHexGridGen)
		AHexGridGen = Cast<AHexGridGenerator>(UGameplayStatics::GetActorOfClass(this, AHexGridGenerator::StaticClass()));

	UpdateHoverWidget();
}

void AHexPlayerController::UpdateHoverWidget() const
{
	if(!HoverWidget || !AHexGridGen || !IsLocalController()) return;

	float MouseX, MouseY;
	if(!GetMousePosition(MouseX, MouseY))
	{
		HoverWidget->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	const FHexHitResult CurrentHoverSelection = AHexGridGen->SendCurrentHoverSelection();

	if(CurrentHoverSelection.SnapType == EHexSnapType::None)
	{
		HoverWidget->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	AHexTiles** HexTiles = AHexGridGen->SpawnedHexTiles.Find(CurrentHoverSelection.ClosestIndex);
	if(!HexTiles || !*HexTiles)
	{
		HoverWidget->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	const AHexTiles* HexTile = *HexTiles;
	const FString ValueAsString = UEnum::GetValueAsString(HexTile->TileType);
	FString LeftString;
	FString RightString;
	ValueAsString.Split(TEXT("::"), &LeftString, &RightString);

	const FText TileType = FText::FromString(RightString + HexTile->TileCoordinates.ToString());
	HoverWidget->TileType->SetTexts(HoverWidget->TileType->HeadingText, TileType);
	HoverWidget->DiceNumber->SetTexts(HoverWidget->DiceNumber->HeadingText, FText::AsNumber(HexTile->DiceNumber));
	HoverWidget->Owner->SetTexts(HoverWidget->Owner->HeadingText, FText::FromString("--"));
	HoverWidget->SetVisibility(ESlateVisibility::Visible);

	int32 ViewX, ViewY;
	GetViewportSize(ViewX, ViewY);

	const FVector2D Cursor(MouseX, MouseY);
	const FVector2D WidgetSize = HoverWidget->GetDesiredSize();

	// Start by placing widget to the bottom-right of cursor
	FVector2D Pos = Cursor + WidgetOffset;

	// Flip horizontally if overflowing right
	if (Pos.X + WidgetSize.X > ViewX)
		Pos.X = Cursor.X - WidgetOffset.X - WidgetSize.X;

	// Flip vertically if overflowing bottom
	if (Pos.Y + WidgetSize.Y > ViewY)
		Pos.Y = Cursor.Y - WidgetOffset.Y - WidgetSize.Y;

	// Final safety clamp
	Pos.X = FMath::Clamp(Pos.X, 0.0f, ViewX - WidgetSize.X);
	Pos.Y = FMath::Clamp(Pos.Y, 0.0f, ViewY - WidgetSize.Y);

	HoverWidget->SetPositionInViewport(Pos);
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

void AHexPlayerController::OnEndTurnPressed()
{
	if(AHexTiles** HexTiles = AHexGridGen->SpawnedHexTiles.Find(SendCurrentHoverSelection.ClosestIndex); !HexTiles || !*HexTiles)
	{
		return;
	}

	EndTurn();
}

void AHexPlayerController::OnTurnIndexChanged()
{
	if(!HudWidget) return;

	const FString TurnOrder = GetTurnOrderString(HexGameState->TurnOrder, HexGameState->CurrentTurnIndex, GetPlayerState<AHexPlayerState>());

	HudWidget->TurnText->SetTexts(HudWidget->TurnText->HeadingText, FText::FromString(TurnOrder));
}

void AHexPlayerController::OnPhaseChanged()
{
	FString Phase = "";

	const FString ValueAsString = UEnum::GetValueAsString(HexGameState->TurnPhase);
	FString TurnPhaseLeftString;
	ValueAsString.Split(TEXT("::"), &TurnPhaseLeftString, &Phase);

	if(HexGameState->TurnPhase == EHexTurnPhase::Setup)
	{
		const FString SetupValueAsString = UEnum::GetValueAsString(HexGameState->SetupRound);
		FString SetupLeftString;
		FString SetupRightString;
		SetupValueAsString.Split(TEXT("::"), &SetupLeftString, &SetupRightString);
		Phase += " " + SetupRightString;
	}

	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Phase Changed %hs %s"), HudWidget ? "True" : "False", *Phase));

	if(!HudWidget) return;
	HudWidget->TurnPhase->SetTexts(HudWidget->TurnPhase->HeadingText, FText::FromString(Phase));
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

	AHexTiles** HexTiles = AHexGridGen->SpawnedHexTiles.Find(SendCurrentHoverSelection.ClosestIndex);
	if(!HexTiles || !*HexTiles)
	{
		return;
	}

	if(!HudWidget || !HudWidget->CurrentSelectedTile) return;
	const FString ValueAsString = UEnum::GetValueAsString(SendCurrentHoverSelection.SnapType);
	FString LeftString;
	FString RightString;
	ValueAsString.Split(TEXT("::"), &LeftString, &RightString);
	const FText TileText = FText::FromString(RightString + SendCurrentHoverSelection.ClosestIndex.ToString());

	HudWidget->CurrentSelectedTile->SetTexts(FText::FromString("Selected Tile"), TileText);

	if(SelectedTile) SelectedTile->OnUnSelected();
	SelectedTile = *HexTiles;
	if(SelectedTile) SelectedTile->OnSelected();
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

FString AHexPlayerController::GetTurnOrderString(const TArray<AHexPlayerState*>& TurnOrder, const int32 CurrentTurnIndex, const AHexPlayerState* LocalPlayerState)
{
	if(TurnOrder.Num() == 0 || !TurnOrder.IsValidIndex(CurrentTurnIndex)) return TEXT("");

	TArray<FString> PlayerNames;
	PlayerNames.Reserve(TurnOrder.Num());

	for (int32 Offset = 0; Offset < TurnOrder.Num(); ++Offset)
	{
		const int32 Index = (CurrentTurnIndex + Offset) % TurnOrder.Num();

		if (const AHexPlayerState* HexPlayerState = TurnOrder[Index])
		{
			FString Name = HexPlayerState->GetPlayerName();

			// Current turn player (first after rotation)
			if (Offset == 0)
			{
				Name += TEXT(" *");
			}

			// Local player
			if (HexPlayerState == LocalPlayerState)
			{
				Name += TEXT(" (YOU)");
			}

			PlayerNames.Add(Name);
		}
	}

	return FString::Join(PlayerNames, TEXT(" -> "));
}
