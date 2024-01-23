// Fill out your copyright notice in the Description page of Project Settings.

#include "DirtLootDefinition.h"

const FDirtLootItem* FDirtLootDefinition::GetItem() const
{
	return Handle.GetRow<FDirtLootItem>("FDirtLootDefinition::GetItem");
}
