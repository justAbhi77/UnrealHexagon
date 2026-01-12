// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HexTypes.h"
#include "GameFramework/PlayerController.h"
#include "HexPlayerController.generated.h"

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
	void SendSelection(const FHexHitResult& Selection);
	void EndTurn(const FInputActionValue& Value);

	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LeftClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* EndTurnAction;
protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_SendSelection(const FHexHitResult& Selection);

	UFUNCTION(Server, Reliable)
	void Server_EndTurn();

	void OnLeftMouseClicked(const FInputActionValue& Value);

	UPROPERTY()
	AHexGridGenerator* AHexGridGen;
};
