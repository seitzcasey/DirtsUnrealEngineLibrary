// Fill out your copyright notice in the Description page of Project Settings.

#include "DirtLootComponent.h"

#include "DirtLootCollection.h"
#include "DirtLootDefinition.h"
#include "DirtLootItem.h"
#include "Engine/AssetManager.h"

UDirtLootComponent::UDirtLootComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

TArray<FDataTableRowHandle> UDirtLootComponent::GenerateLoot()
{
	if ensureAlways(Loot)
	{
		return Loot->GenerateLoot();	
	}
	return {};
}
