// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DirtLootCollection.generated.h"

struct FStreamableHandle;
DECLARE_DELEGATE_OneParam(FLoadLootDefinitionsDelegate, bool /*bSuccess*/);

UCLASS()
class LOOT_API UDirtLootCollection : public UDataAsset
{
	GENERATED_BODY()

protected:
#pragma region Properties
#pragma region UProperties
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Loot")
	TMap<int32, float> NumRollChance;
	
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Loot")
	TArray<TSoftObjectPtr<UDataTable>> LootDefinitions;
#pragma endregion UProperties

#pragma region Delegates
#pragma endregion Delegates

#pragma region Non-UProperties
	bool bLoadedDefinitions = false;
#pragma endregion Non-UProperties
#pragma endregion Properties

public:
#pragma region PublicMethods
	UDirtLootCollection();

	void LoadLootDefinitions();

	void LoadLootDefinitions_Async(const FLoadLootDefinitionsDelegate& Callback);

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
	TSharedPtr<FStreamableHandle> LoadLootDefinitions_Internal(bool bAsync);

	int32 GetNumRolls() const;
#pragma region ProtectedParentInterfaces
#pragma endregion ProtectedParentInterfaces
#pragma endregion ProtectedMethods
	
};
