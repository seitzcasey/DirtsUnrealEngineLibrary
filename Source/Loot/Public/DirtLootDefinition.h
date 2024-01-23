// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DirtLootDefinition.generated.h"

struct FDirtLootItem;

USTRUCT(BlueprintType)
struct LOOT_API FDirtLootDefinition : public FTableRowBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Loot")
	float Chance = 1.f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Loot", meta = (RowType = "DirtLootItem", ShowOnlyInnerProperties))
	FDataTableRowHandle Handle;

public:
	float GetChance() const { return Chance; }

	const FDataTableRowHandle& GetHandle() const { return Handle; }

	const FDirtLootItem* GetItem() const;
};
