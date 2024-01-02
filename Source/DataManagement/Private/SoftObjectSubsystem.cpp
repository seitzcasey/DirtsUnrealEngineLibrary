// Fill out your copyright notice in the Description page of Project Settings.

#include "SoftObjectSubsystem.h"
#include "Engine/StreamableManager.h"

TArray<FSoftObjectPath> USoftObjectCollection::GetSoftObjectsFromProperty(FProperty* Property, const void* PropertyValue)
{
	TArray<FSoftObjectPath> Output;

	if (auto const* SoftClassProp = CastField<FSoftClassProperty>(Property))
	{
		if (auto const* Value = SoftClassProp->GetPropertyValuePtr(PropertyValue))
		{
			Output.Add(Value->ToSoftObjectPath());
		}
	}
	else if (auto const* SoftObjectProp = CastField<FSoftObjectProperty>(Property))
	{
		if (auto const* Value = SoftObjectProp->GetPropertyValuePtr(PropertyValue))
		{
			Output.Add(Value->ToSoftObjectPath());
		}
	}
	else if (auto const* StructProp = CastField<FStructProperty>(Property))
	{
		for (TFieldIterator<FProperty> It(StructProp->Struct); It; ++It)
		{
			void const* StructMemberPtr = It->ContainerPtrToValuePtr<void>(PropertyValue);
			TArray<FSoftObjectPath> InnerPaths = GetSoftObjectsFromProperty(*It, StructMemberPtr);
			Output.Append(InnerPaths);
		}
	}

	return Output;
}

TArray<FSoftObjectPath> USoftObjectCollection::GetSoftObjectsToLoad() const
{
	TArray<FSoftObjectPath> Output;
	
	for (TFieldIterator<FProperty> It(GetClass()); It; ++It)
	{
		FProperty* Property = *It;
		void const* PropertyValue = Property->ContainerPtrToValuePtr<void>(this);
		TArray<FSoftObjectPath> PropertyPaths = GetSoftObjectsFromProperty(Property, PropertyValue);
		Output.Append(PropertyPaths);
	}

	return Output;
}

USoftObjectSettings* USoftObjectSettings::Get()
{
	return StaticClass()->GetDefaultObject<USoftObjectSettings>();
}

USoftObjectSubsystem* USoftObjectSubsystem::Get()
{
	return GEngine->GetEngineSubsystem<USoftObjectSubsystem>();
}

void USoftObjectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	InitializeAllCollections();
	
	// Load AlwaysLoaded Collections
	TArray<FSoftObjectPath> SoftObjectPaths;
	GetAlwaysLoadedSoftObjectsToLoad(SoftObjectPaths);

	LoadSoftObjects(SoftObjectPaths, AlwaysLoadedCollectionName);
	
	Super::Initialize(Collection);
}

TSharedPtr<FStreamableHandle> USoftObjectSubsystem::LoadSoftObjectCollection(const FName CollectionName)
{
#if WITH_EDITOR
	if (CollectionName == AlwaysLoadedCollectionName)
	{
		// Error Message
	}
#endif
	
	TArray<FSoftObjectPath> SoftObjectPaths;
	GetSoftObjectsToLoad(SoftObjectPaths, CollectionName);
	
	return LoadSoftObjects(SoftObjectPaths, CollectionName);
}

void USoftObjectSubsystem::UnloadSoftObjectCollection(const FName CollectionName)
{
	if (auto const* HandlePtr = LoadedCollections.Find(CollectionName))
	{
		auto const& Handle = *HandlePtr;
		Handle->ReleaseHandle();

		LoadedCollections.Remove(CollectionName);
	}
}

USoftObjectCollection* USoftObjectSubsystem::K2_GetLoadedCollectionByID(const FName ID) const
{
	// If we didn't receive a CollectionName, check em all.
	for (const auto Collection : AllCollections)
	{
		if (Collection->ID == ID)
		{
			return Collection;
		}
	}

	return nullptr;
}

void USoftObjectSubsystem::InitializeAllCollections()
{
	const int32 NumAlwaysLoadedCollections = USoftObjectSettings::Get()->AlwaysLoadedCollections.Num();
	const int32 NumOnDemandCollections = USoftObjectSettings::Get()->OnDemandCollections.Num();
	
	TArray<FSoftObjectPath> CollectionPaths;
	CollectionPaths.Reserve(NumAlwaysLoadedCollections + NumOnDemandCollections);
	
	for (auto const Collection : USoftObjectSettings::Get()->AlwaysLoadedCollections)
	{
		CollectionPaths.Add(Collection.ToSoftObjectPath());
	}

	for (auto const& [Name, Collection] : USoftObjectSettings::Get()->OnDemandCollections)
	{
		CollectionPaths.Add(Collection.ToSoftObjectPath());
	}
	
	StreamableManager.RequestSyncLoad(CollectionPaths);

	AllCollections.Empty(CollectionPaths.Num());
	
	for (auto const& Path : CollectionPaths)
	{
		AllCollections.Add(Cast<USoftObjectCollection>(Path.ResolveObject()));
	}
}

void USoftObjectSubsystem::GetSoftObjectsToLoad(TArray<FSoftObjectPath>& SoftObjectPaths,
                                                const FName CollectionName) const
{
	if (auto const* CollectionPtr = USoftObjectSettings::Get()->OnDemandCollections.Find(CollectionName))
	{
		const auto Collection = *CollectionPtr;
		if ensureAlways(Collection.IsValid())
		{
			SoftObjectPaths.Append(Collection->GetSoftObjectsToLoad());	
		}
	}
}

void USoftObjectSubsystem::GetAlwaysLoadedSoftObjectsToLoad(TArray<FSoftObjectPath>& SoftObjectPaths) const
{
	for (const auto Collection : USoftObjectSettings::Get()->AlwaysLoadedCollections)
	{
		if ensureAlways(Collection.IsValid())
		{
			SoftObjectPaths.Append(Collection->GetSoftObjectsToLoad());	
		}
	}
}

TSharedPtr<FStreamableHandle> USoftObjectSubsystem::LoadSoftObjects(const TArray<FSoftObjectPath>& SoftObjectPaths, const FName CollectionName)
{
	if (SoftObjectPaths.IsEmpty())
	{
		return nullptr;
	}
	
	auto Handle = StreamableManager.RequestAsyncLoad(SoftObjectPaths, []()
	{
		// Maybe add callback?
	}, FStreamableManager::DefaultAsyncLoadPriority, true);

	LoadedCollections.FindOrAdd(CollectionName) = Handle;
	return Handle;
}
