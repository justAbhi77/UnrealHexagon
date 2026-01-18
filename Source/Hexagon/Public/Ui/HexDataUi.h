// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HexDataUi.generated.h"

class UTextBlock;

/**
 *
 */
UCLASS()
class HEXAGON_API UHexDataUi : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText HeadingText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText BodyText;

	UFUNCTION(BlueprintCallable)
	void SetTexts(const FText& InHeadingText, const FText& InBodyText);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true", BindWidget))
	TObjectPtr<UTextBlock> Heading;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true", BindWidget))
	TObjectPtr<UTextBlock> Body;
};
