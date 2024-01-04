// Fill out your copyright notice in the Description page of Project Settings.

#include "DirtButton.h"
#include "Components/TextBlock.h"

UDirtButton::UDirtButton()
{
}

void UDirtButton::SetText(const FText& InText)
{
	ButtonText = InText;
	
	if ensureAlways(IsValid(TextBlock))
	{
		TextBlock->SetText(ButtonText);	
	}
}

void UDirtButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (!ButtonText.IsEmpty())
	{
		SetText(ButtonText);	
	}
	else if (IsValid(TextBlock))
	{
		ButtonText = TextBlock->GetText();
	}
}
