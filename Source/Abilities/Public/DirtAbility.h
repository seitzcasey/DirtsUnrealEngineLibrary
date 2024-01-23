// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"

#include "DirtAbility.generated.h"

class UInputAction;

UCLASS(Abstract)
class ABILITIES_API UDirtAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dirt Ability")
	TObjectPtr<UInputAction> InputAction;
	
public:
	UDirtAbility();

	UInputAction* GetInputAction() const { return InputAction; }
};
