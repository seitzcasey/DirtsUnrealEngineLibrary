// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "DirtButton.generated.h"

class UTextBlock;

UCLASS()
class USERINTERFACE_API UDirtButton : public UCommonButtonBase
{
	GENERATED_BODY()

protected:
#pragma region Properties
    
#pragma region UProperties
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Text")
	FText ButtonText;
	
#pragma endregion UProperties

#pragma region Delegates
#pragma endregion Delegates

#pragma region Non-UProperties
#pragma endregion Non-UProperties

#pragma endregion Properties

public:
#pragma region PublicMethods
	UDirtButton();

#pragma region Setters
	void SetText(const FText& InText);
	
#pragma endregion Setters

#pragma region Getters
#pragma endregion Getters

#pragma region PublicParentInterfaces
	virtual void NativePreConstruct() override;
	
#pragma endregion PublicParentInterfaces

#pragma endregion PublicMethods

protected:
#pragma region ProtectedMethods

#pragma region ProtectedParentInterfaces
#pragma endregion ProtectedParentInterfaces

#pragma endregion ProtectedMethods
};
