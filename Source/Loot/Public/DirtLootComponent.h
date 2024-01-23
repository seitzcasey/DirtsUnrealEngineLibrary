// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DirtLootComponent.generated.h"


class UDirtLootCollection;
struct FStreamableHandle;
class UDirtLootDefinition;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LOOT_API UDirtLootComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
#pragma region Properties
#pragma region UProperties
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Loot")
	TObjectPtr<UDirtLootCollection> Loot;
#pragma endregion UProperties

#pragma region Delegates
#pragma endregion Delegates

#pragma region Non-UProperties
#pragma endregion Non-UProperties
#pragma endregion Properties

public:
#pragma region PublicMethods
	UDirtLootComponent();
	
	TArray<FDataTableRowHandle> GenerateLoot();

#pragma region Setters
#pragma endregion Setters

#pragma region Getters
#pragma endregion Getters

#pragma region PublicParentInterfaces
#pragma endregion PublicParentInterfaces
#pragma endregion PublicMethods

protected:
#pragma region ProtectedMethods
#pragma region ProtectedParentInterfaces
#pragma endregion ProtectedParentInterfaces
#pragma endregion ProtectedMethods
};
