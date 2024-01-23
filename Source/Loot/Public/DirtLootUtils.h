// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "UObject/Object.h"
#include "DirtLootUtils.generated.h"

UCLASS()
class LOOT_API UDirtLootUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	template<typename TKey>
	static TArray<TKey> GetKeysByCumulativeChance(const TMap<TKey, float>& ChanceMap, int32 Quantity, float TotalChance = 0.f)
	{
		TArray<TKey> SelectedKeys;

		// Calculate total chance
		if (TotalChance == 0.f)
		{
			for (const auto& Elem : ChanceMap)
			{
				TotalChance += Elem.Value;
			}	
		}

		// Check if the map is empty or Quantity is 0
		if (ChanceMap.Num() == 0 || Quantity <= 0)
		{
			return SelectedKeys;
		}

		for (int32 i = 0; i < Quantity; ++i)
		{
			float RandomChance = FMath::FRandRange(0.0f, TotalChance);
			float CumulativeChance = 0.0f;

			for (const auto& Elem : ChanceMap)
			{
				CumulativeChance += Elem.Value;
				if (RandomChance <= CumulativeChance)
				{
					SelectedKeys.Add(Elem.Key);
					break;
				}
			}
		}

		return SelectedKeys;
	}

	template<typename TKey>
	static bool GetKeyByCumulativeChance(OUT TKey& Key, const TMap<TKey, float>& ChanceMap, float TotalChance = 0.f)
	{
		const auto Output = GetKeysByCumulativeChance(ChanceMap, 1, TotalChance);
		if (!Output.IsEmpty())
		{
			Key = Output[0];
			return true;
		}
		return false;
	}
};
