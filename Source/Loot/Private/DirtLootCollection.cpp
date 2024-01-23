// Fill out your copyright notice in the Description page of Project Settings.


#include "DirtLootCollection.h"

#include "DirtLootDefinition.h"
#include "DirtLootUtils.h"
#include "Engine/AssetManager.h"

uint32 GetTypeHash(const FDataTableRowHandle& Handle)
{
	return HashCombine(
		GetTypeHash(Handle.DataTable),
		GetTypeHash(Handle.RowName)
	);
}

UDirtLootCollection::UDirtLootCollection()
{
	NumRollChance = { {1, 1.f} };
}

void UDirtLootCollection::LoadLootDefinitions()
{
}

void UDirtLootCollection::LoadLootDefinitions_Async(const FLoadLootDefinitionsDelegate& Callback)
{
}

TArray<FDataTableRowHandle> UDirtLootCollection::GenerateLoot()
{
	float TotalChance = 0.f;
	TMap<FDataTableRowHandle, float> ChanceMap;

	const TFunctionRef<void(const FName&, const FDirtLootDefinition&)> GetLootDefinition = [&TotalChance, &ChanceMap](const FName& RowName, const FDirtLootDefinition& Definition)
	{
		TotalChance += Definition.GetChance();
		ChanceMap.Add(Definition.GetHandle(), Definition.GetChance());
	};
	
	if ensureAlways(bLoadedDefinitions)
	{
		for (const auto SoftDefinition : LootDefinitions)
		{
			auto const* Definition = SoftDefinition.Get();
			ChanceMap.Reserve(Definition->GetRowMap().Num());
			Definition->ForeachRow("UDirtLootComponent::GenerateLoot", GetLootDefinition);
		}
	}
	
	return UDirtLootUtils::GetKeysByCumulativeChance(ChanceMap, GetNumRolls(), TotalChance);
}

TSharedPtr<FStreamableHandle> UDirtLootCollection::LoadLootDefinitions_Internal(bool bAsync)
{
	auto& StreamableManager = UAssetManager::GetStreamableManager();

	TArray<FSoftObjectPath> DefinitionPaths;
	for (const auto Definition : LootDefinitions)
	{
		if (Definition.IsValid() || Definition.IsPending())
		{
			DefinitionPaths.Add(Definition.ToSoftObjectPath());	
		}
	}
	
	if (bAsync)
	{
		return StreamableManager.RequestAsyncLoad(DefinitionPaths);
	}
	else
	{
		return StreamableManager.RequestSyncLoad(DefinitionPaths);
	}
}

int32 UDirtLootCollection::GetNumRolls() const
{
	// Prepare a sorted array of cumulative chances and associated numbers
	TArray<TPair<float, int32>> CumulativeChances;
	float TotalChance = 0.0f;

	for (const TPair<int32, float>& Pair : NumRollChance)
	{
		TotalChance += Pair.Value;
		CumulativeChances.Add(TPair<float, int32>(TotalChance, Pair.Key));
	}

	// Sort based on the cumulative chance
	CumulativeChances.Sort([](const TPair<float, int32>& A, const TPair<float, int32>& B)
	{
		return A.Key < B.Key;
	});

	// Generate a random number and find the corresponding number
	float RandomChance = FMath::FRand() * TotalChance; // Scale random chance to total chance
	for (const TPair<float, int32>& ChancePair : CumulativeChances)
	{
		if (RandomChance <= ChancePair.Key)
		{
			return ChancePair.Value;
		}
	}

	return 0; // Default return value if no match is found
}
