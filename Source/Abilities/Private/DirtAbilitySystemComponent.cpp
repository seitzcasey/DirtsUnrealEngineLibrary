// Fill out your copyright notice in the Description page of Project Settings.

#include "DirtAbilitySystemComponent.h"
#include "DirtAbility.h"
#include "EnhancedInputComponent.h"

namespace UDirtAbilitySystemComponent_Impl
{
	constexpr int32 InvalidInputID = 0;
	int32 IncrementingInputID = InvalidInputID;

	static int32 GetNextInputID()
	{
		return ++IncrementingInputID;
	}
}

UDirtAbilitySystemComponent::UDirtAbilitySystemComponent()
{
}

void UDirtAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	using namespace UDirtAbilitySystemComponent_Impl;
	
	Super::OnGiveAbility(AbilitySpec);

	const UDirtAbility* Ability = Cast<UDirtAbility>(AbilitySpec.Ability);
	if (UInputAction* Action = Ability ? Ability->GetInputAction() : nullptr)
	{
		if (auto* InputBinding = AbilityInputMapping.Find(Action))
		{
			InputBinding->BoundAbilitiesStack.Remove(AbilitySpec.Handle);
			InputBinding->BoundAbilitiesStack.Push(AbilitySpec.Handle);
		}
		else if (auto* InputComponent = GetOwnerInputComponent())
		{
			FDirtAbilityInputBinding Binding;
			Binding.InputID = GetNextInputID();
			Binding.OnPressedHandle = InputComponent->BindAction(Action, ETriggerEvent::Started, this, &ThisClass::OnAbilityInputPressed, Action).GetHandle();
			Binding.OnReleasedHandle = InputComponent->BindAction(Action, ETriggerEvent::Completed, this, &ThisClass::OnAbilityInputReleased, Action).GetHandle();
			Binding.BoundAbilitiesStack.Push(AbilitySpec.Handle);
			AbilityInputMapping.Add(Action, std::move(Binding));

			AbilitySpec.InputID = Binding.InputID;
		}
	}
}

void UDirtAbilitySystemComponent::OnAbilityInputPressed(UInputAction* Action)
{
	if (auto const* Binding = AbilityInputMapping.Find(Action))
	{
		AbilityLocalInputPressed(Binding->InputID);
	}
}

void UDirtAbilitySystemComponent::OnAbilityInputReleased(UInputAction* Action)
{
	if (auto const* Binding = AbilityInputMapping.Find(Action))
	{
		AbilityLocalInputReleased(Binding->InputID);
	}
}

UEnhancedInputComponent* UDirtAbilitySystemComponent::GetOwnerInputComponent() const
{
	auto const* Owner = GetOwner();
	return Owner ? Owner->GetComponentByClass<UEnhancedInputComponent>() : nullptr;
}
