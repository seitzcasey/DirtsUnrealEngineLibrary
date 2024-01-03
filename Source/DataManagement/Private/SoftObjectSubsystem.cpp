// Fill out your copyright notice in the Description page of Project Settings.

#include "SoftObjectSubsystem.h"
#include "AlwaysLoadedSoftObjectCollection.h"
#include "OnDemandSoftObjectCollection.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(DataManagement_Collection, "DataManagement.Collection", "Root Tag for CollectionTags");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(DataManagement_ID, "DataManagement.ID", "Root Tag for SoftObject IDs");

UObject* USoftObjectCollection::K2_GetObject(FGameplayTag ID) const
{
	auto const* ObjectPtr = Objects.Find(ID);
	if ensureAlways(ObjectPtr)
	{
		return ObjectPtr->Get();
	}
	return nullptr;
}

TSubclassOf<UObject> USoftObjectCollection::K2_GetClass(FGameplayTag ID) const
{
	auto const* ClassPtr = Classes.Find(ID);
	if ensureAlways(ClassPtr)
	{
		return ClassPtr->Get();
	}
	return nullptr;
}

TArray<FSoftObjectPath> USoftObjectCollection::GetSoftObjectsFromProperty(FProperty* Property, const void* PropertyValue)
{
    TArray<FSoftObjectPath> Output;

	if (auto const* MapProp = CastField<FMapProperty>(Property))
	{
		FScriptMapHelper MapHelper(MapProp, PropertyValue);
		for (int32 Index = 0; Index < MapHelper.GetMaxIndex(); ++Index)
		{
			// Process the key
			TArray<FSoftObjectPath> KeyPaths = GetSoftObjectsFromProperty(MapProp->KeyProp, MapHelper.GetKeyPtr(Index));
			Output.Append(KeyPaths);

			// Process the value
			TArray<FSoftObjectPath> ValuePaths = GetSoftObjectsFromProperty(MapProp->ValueProp, MapHelper.GetValuePtr(Index));
			Output.Append(ValuePaths);
		}
	}
    else if (auto const* SoftClassProp = CastField<FSoftClassProperty>(Property))
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
    else if (auto const* ArrayProp = CastField<FArrayProperty>(Property))
    {
        FScriptArrayHelper ArrayHelper(ArrayProp, PropertyValue);
        for (int32 Index = 0; Index < ArrayHelper.Num(); ++Index)
        {
            void* ElementPtr = ArrayHelper.GetRawPtr(Index);
            TArray<FSoftObjectPath> InnerPaths = GetSoftObjectsFromProperty(ArrayProp->Inner, ElementPtr);
            Output.Append(InnerPaths);
        }
    }

    return Output;
}

TArray<FSoftObjectPath> USoftObjectCollection::GetSoftObjectsToLoad() const
{
	TArray<FSoftObjectPath> Output;
	
	for (TFieldIterator<FProperty> It(UObject::GetClass()); It; ++It)
	{
		FProperty* Property = *It;
		void const* PropertyValue = Property->ContainerPtrToValuePtr<void>(this);
		TArray<FSoftObjectPath> PropertyPaths = GetSoftObjectsFromProperty(Property, PropertyValue);
		Output.Append(PropertyPaths);
	}

	return Output;
}

void USoftObjectCollection::CheckID(const FGameplayTag& ID)
{
	ensureAlways(ID.MatchesTag(DataManagement_ID));
}

USoftObjectSettings* USoftObjectSettings::Get()
{
	return StaticClass()->GetDefaultObject<USoftObjectSettings>();
}

USoftObjectSubsystem* USoftObjectSubsystem::Get(const UObject* WorldContextObject)
{
	auto const* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	auto const* GI = World ? World->GetGameInstance() : nullptr;
	return GI ? GI->GetSubsystem<USoftObjectSubsystem>() : nullptr;
}

void USoftObjectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	InitializeAllCollections();
	LoadAlwaysLoadedCollections();	
	
	Super::Initialize(Collection);
}

TSharedPtr<FStreamableHandle> USoftObjectSubsystem::LoadSoftObjectCollection(const FGameplayTag& CollectionTag)
{
	CheckCollectionTag(CollectionTag);
	
	TArray<FSoftObjectPath> SoftObjectPaths;
	GetSoftObjectsToLoad(CollectionTag, SoftObjectPaths);
	
	return LoadSoftObjects(CollectionTag, SoftObjectPaths);
}

bool USoftObjectSubsystem::K2_LoadSoftObjectCollection(FGameplayTag CollectionTag)
{
	return LoadSoftObjectCollection(CollectionTag) != nullptr;
}

bool USoftObjectSubsystem::UnloadSoftObjectCollection(const FGameplayTag& CollectionTag)
{
	CheckCollectionTag(CollectionTag);
	
	if (auto const* HandlePtr = LoadedCollections.Find(CollectionTag))
	{
		auto const& Handle = *HandlePtr;
		Handle->ReleaseHandle();

		LoadedCollections.Remove(CollectionTag);
		return true;
	}
	return false;
}

bool USoftObjectSubsystem::K2_UnloadSoftObjectCollection(FGameplayTag CollectionTag)
{
	return UnloadSoftObjectCollection(CollectionTag);
}

USoftObjectCollection* USoftObjectSubsystem::GetLoadedCollection(const FGameplayTag& CollectionTag) const
{
	CheckCollectionTag(CollectionTag);
	
	if ensureAlways(IsCollectionLoaded(CollectionTag))
	{
		if (auto const* CollectionPtr = USoftObjectSettings::Get()->AlwaysLoadedCollections.Find(CollectionTag))
		{
			return CollectionPtr->Get();
		}

		if (auto const* CollectionPtr = USoftObjectSettings::Get()->OnDemandCollections.Find(CollectionTag))
		{
			return CollectionPtr->Get();
		}
	}

	return nullptr;
}

USoftObjectCollection* USoftObjectSubsystem::K2_GetLoadedCollection(FGameplayTag CollectionTag) const
{
	return GetLoadedCollection(CollectionTag);
}

bool USoftObjectSubsystem::IsCollectionLoaded(const FGameplayTag& CollectionTag) const
{
	CheckCollectionTag(CollectionTag);
	return LoadedCollections.Find(CollectionTag) != nullptr;
}

bool USoftObjectSubsystem::K2_IsCollectionLoaded(FGameplayTag CollectionTag) const
{
	return IsCollectionLoaded(CollectionTag);
}

void USoftObjectSubsystem::InitializeAllCollections()
{
	const int32 NumAlwaysLoadedCollections = USoftObjectSettings::Get()->AlwaysLoadedCollections.Num();
	const int32 NumOnDemandCollections = USoftObjectSettings::Get()->OnDemandCollections.Num();
	
	TArray<FSoftObjectPath> CollectionPaths;
	CollectionPaths.Reserve(NumAlwaysLoadedCollections + NumOnDemandCollections);
	
	for (auto const& [CollectionTag, Collection] : USoftObjectSettings::Get()->AlwaysLoadedCollections)
	{
		CollectionPaths.Add(Collection.ToSoftObjectPath());
	}

	for (auto const& [CollectionTag, Collection] : USoftObjectSettings::Get()->OnDemandCollections)
	{
		CollectionPaths.Add(Collection.ToSoftObjectPath());
	}
	
	StreamableManager.RequestSyncLoad(CollectionPaths);

	InitializedCollections.Empty(CollectionPaths.Num());
	
	for (auto const& Path : CollectionPaths)
	{
		InitializedCollections.Add(Cast<USoftObjectCollection>(Path.ResolveObject()));
	}
}

void USoftObjectSubsystem::GetSoftObjectsToLoad(const FGameplayTag& CollectionTag, TArray<FSoftObjectPath>& SoftObjectPaths) const
{
	auto const* CollectionPtr = USoftObjectSettings::Get()->OnDemandCollections.Find(CollectionTag);
	if ensureAlways(CollectionPtr)
	{
		const auto Collection = *CollectionPtr;
		if ensureAlways(Collection.IsValid())
		{
			SoftObjectPaths.Append(Collection->GetSoftObjectsToLoad());	
		}
	}
}

void USoftObjectSubsystem::GetAlwaysLoadedSoftObjectsToLoad(TMap<FGameplayTag, TArray<FSoftObjectPath>>& SoftObjectPaths) const
{
	for (auto const& [CollectionTag, Collection] : USoftObjectSettings::Get()->AlwaysLoadedCollections)
	{
		if ensureAlways(Collection.IsValid())
		{
			SoftObjectPaths.Add(CollectionTag, Collection->GetSoftObjectsToLoad());	
		}
	}
}

TSharedPtr<FStreamableHandle> USoftObjectSubsystem::LoadSoftObjects(const FGameplayTag& CollectionTag, const TArray<FSoftObjectPath>& SoftObjectPaths)
{
	if (SoftObjectPaths.IsEmpty())
	{
		return nullptr;
	}
	
	auto Handle = StreamableManager.RequestAsyncLoad(SoftObjectPaths, []()
	{
		// Maybe add callback?
	}, FStreamableManager::DefaultAsyncLoadPriority, true);

	LoadedCollections.FindOrAdd(CollectionTag) = Handle;
	return Handle;
}

void USoftObjectSubsystem::CheckCollectionTag(const FGameplayTag& CollectionTag)
{
	ensureAlways(CollectionTag.MatchesTag(DataManagement_Collection));
}

void USoftObjectSubsystem::LoadAlwaysLoadedCollections()
{
	TMap<FGameplayTag, TArray<FSoftObjectPath>> SoftObjectPaths;
	GetAlwaysLoadedSoftObjectsToLoad(SoftObjectPaths);

	for (auto const& [CollectionTag, Paths] : SoftObjectPaths)
	{
		LoadSoftObjects(CollectionTag, Paths);	
	}
}
