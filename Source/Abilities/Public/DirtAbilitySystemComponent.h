// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DirtAbilitySystemComponent.generated.h"

class UInputAction;
class UEnhancedInputComponent;

struct FDirtAbilityInputBinding
{
	int32  InputID = 0;
	uint32 OnPressedHandle = 0;
	uint32 OnReleasedHandle = 0;
	TArray<FGameplayAbilitySpecHandle> BoundAbilitiesStack;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ABILITIES_API UDirtAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:
	TMap<TObjectPtr<UInputAction>, FDirtAbilityInputBinding> AbilityInputMapping;
	
public:
	UDirtAbilitySystemComponent();

	//~ Being AbilitySystemComponent Interface
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	//~ End AbilitySystemComponent Interface

protected:
	void OnAbilityInputPressed(UInputAction* Action);

	void OnAbilityInputReleased(UInputAction* Action);
	
	UEnhancedInputComponent* GetOwnerInputComponent() const;
};
