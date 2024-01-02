// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrimaryGameLayout.h"
#include "DirtGameLayout.generated.h"

class UCommonActivatableWidgetStack;

/**
 * How to use:
 * 1. Create a BP child of GameUIManager.
 * 2. Create a BP child of GameUIPolicy. Set DefaultPolicyClass in your DefaultGame.ini like below
 *
 * [/Script/CommonGame.GameUIManagerSubsystem]
 * DefaultUIPolicyClass=/Game/{PATH_TO_BP}/{BP_Policy}.{BP_Policy}_C
 * 
 * 3. Create a BP child of DirtGameLayout. Add an overlay widget and 3 CommonActivatableWidgetStack widgets and label them appropriately.
 * (GameStack, MenuStack, ModalStack)
*/

UCLASS()
class USERINTERFACE_API UDirtGameLayout : public UPrimaryGameLayout
{
	GENERATED_BODY()
protected:
#pragma region Properties
    
#pragma region UProperties
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> GameStack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> MenuStack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> ModalStack;
	
#pragma endregion UProperties

#pragma region Delegates
#pragma endregion Delegates

#pragma region Non-UProperties
#pragma endregion Non-UProperties

#pragma endregion Properties

public:
#pragma region PublicMethods

#pragma region Setters
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
