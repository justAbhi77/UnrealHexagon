// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HexTypes.h"
#include "GameFramework/PlayerController.h"
#include "HexPlayerController.generated.h"

class UHexHudUi;
class UHexHoverUi;
class AHexGridGenerator;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

UCLASS()
class HEXAGON_API AHexPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AHexPlayerController();

	/* ===== Client Input ===== */
	void ActionEndTurn(const FInputActionValue& Value);

	void EndTurn();

	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LeftClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* EndTurnAction;

	virtual void Tick(float DeltaSeconds) override;

	void UpdateHoverWidget() const;
protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_EndTurn(const FHexHitResult& InSelection);

	void OnLeftMouseClicked(const FInputActionValue& Value);

	UPROPERTY()
	AHexGridGenerator* AHexGridGen;

	FHexHitResult SendCurrentHoverSelection;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HoverWidgetClass;

	UPROPERTY()
	UHexHoverUi* HoverWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHexHoverUi> HudWidgetClass;

	UPROPERTY()
	UHexHudUi* HudWidget;
};
