// Fill out your copyright notice in the Description page of Project Settings.


#include "Ui/HexDataUi.h"
#include "Components/TextBlock.h"

void UHexDataUi::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(Heading)
		Heading->SetText(HeadingText);

	if(Body)
		Body->SetText(BodyText);
}

void UHexDataUi::SetTexts(const FText& InHeadingText, const FText& InBodyText)
{
	HeadingText = InHeadingText;
	BodyText = InBodyText;

	if(Heading)
		Heading->SetText(HeadingText);

	if(Body)
		Body->SetText(BodyText);
}
