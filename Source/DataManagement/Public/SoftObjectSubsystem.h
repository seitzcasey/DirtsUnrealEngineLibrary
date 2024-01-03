// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"
#include "Engine/StreamableManager.h"
#include "SoftObjectSubsystem.generated.h"

class UOnDemandSoftObjectCollection;
class UAlwaysLoadedSoftObjectCollection;
struct FStreamableHandle;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataManagement_Collection);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataManagement_ID);

UCLASS(Abstract)
class DATAMANAGEMENT_API USoftObjectCollection : public UDataAsset
{
	GENERATED_BODY()
	
	friend class USoftObjectSubsystem;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SoftObjectCollection", meta = (Categories = "DataManagement.ID"))
	TMap<FGameplayTag, TSoftObjectPtr<UObject>> Objects;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SoftObjectCollection", meta = (Categories = "DataManagement.ID"))
	TMap<FGameplayTag, TSoftClassPtr<UObject>> Classes;

public:
	template<typename T>
	T* GetObject(const FGameplayTag& ID) const
	{
		CheckID(ID);
		return Cast<T>(K2_GetObject(ID));
	}

	template<typename T>
	TSubclassOf<T> GetClass(const FGameplayTag& ID) const
	{
		CheckID(ID);
		return TSubclassOf<T>(K2_GetClass(ID));
	}

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Object"))
	UObject* K2_GetObject(UPARAM(meta = (Categories = "DataManagement.ID")) FGameplayTag ID) const;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Class"))
	TSubclassOf<UObject> K2_GetClass(UPARAM(meta = (Categories = "DataManagement.ID")) FGameplayTag ID) const;
	
private:
	/**
	 * @brief Extracts soft object paths from a given property.
	 *
	 * This method handles FSoftClassProperty, FSoftObjectProperty, and
	 * recursively processes FStructProperty to find nested soft objects or classes.
	 *
	 * @param Property The property to examine.
	 * @param PropertyValue A pointer to the value of the property.
	 * @return TArray<FSoftObjectPath> A list of soft object paths extracted from the property.
	 */
	static TArray<FSoftObjectPath> GetSoftObjectsFromProperty(FProperty* Property, const void* PropertyValue);

	/**
	 * @brief Retrieves a list of soft object paths to be loaded for this object.
	 *
	 * Iterates over all properties of the object's class and gathers soft object paths
	 * using GetSoftObjectsFromProperty.
	 *
	 * @return TArray<FSoftObjectPath> A list of soft object paths to load.
	 */
	TArray<FSoftObjectPath> GetSoftObjectsToLoad() const;

	static void CheckID(const FGameplayTag& ID);
};

/** Settings for the SoftObjectSubsystem, these settings are used manage collections of static and dynamic data sets to be loaded during runtime.*/
UCLASS(Config=Game, meta = (DisplayName = "Data Management"))
class DATAMANAGEMENT_API USoftObjectSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	friend class USoftObjectSubsystem;

protected:
	UPROPERTY(Config, EditAnywhere, Category = "Data Management", meta = (Categories = "DataManagement.Collection"))
	TMap<FGameplayTag, TSoftObjectPtr<UAlwaysLoadedSoftObjectCollection>> AlwaysLoadedCollections;
	
	UPROPERTY(Config, EditAnywhere, Category = "Data Management", meta = (Categories = "DataManagement.Collection"))
	TMap<FGameplayTag, TSoftObjectPtr<UOnDemandSoftObjectCollection>> OnDemandCollections;

public:
	static USoftObjectSettings* Get();
};

UCLASS()
class DATAMANAGEMENT_API USoftObjectSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
protected:
	FStreamableManager StreamableManager;
	
	// Map to store loaded collections and their objects
	TMap<FGameplayTag, TSharedPtr<FStreamableHandle>> LoadedCollections;

	// Protect collections from GC.
	UPROPERTY()
	TArray<TObjectPtr<USoftObjectCollection>> InitializedCollections;
	
public:	
	/**
	 * @brief Retrieves the instance of USoftObjectSubsystem.
	 *
	 * Accesses the USoftObjectSubsystem instance from the global engine instance.
	 *
	 * @return USoftObjectSubsystem* Pointer to the USoftObjectSubsystem instance.
	 */
	static USoftObjectSubsystem* Get(const UObject* WorldContextObject);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Method to load a specific collection of soft objects
	TSharedPtr<FStreamableHandle> LoadSoftObjectCollection(const FGameplayTag& CollectionTag);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Load Soft Object Collection"))
	bool K2_LoadSoftObjectCollection(UPARAM(meta = (Categories = "DataManagement.Collection")) FGameplayTag CollectionTag);
	
	bool UnloadSoftObjectCollection(const FGameplayTag& CollectionTag);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Unload Soft Object Collection"))
	bool K2_UnloadSoftObjectCollection(FGameplayTag CollectionTag);

	USoftObjectCollection* GetLoadedCollection(const FGameplayTag& CollectionTag) const;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Loaded Collection"))
	USoftObjectCollection* K2_GetLoadedCollection(UPARAM(meta = (Categories = "DataManagement.Collection")) FGameplayTag CollectionTag) const;

	bool IsCollectionLoaded(const FGameplayTag& CollectionTag) const;
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is Collection Loaded"))
	bool K2_IsCollectionLoaded(UPARAM(meta = (Categories = "DataManagement.Collection")) FGameplayTag CollectionTag) const;

protected:
	// Loads all collection containers into memory so they're ready for dynamic load/unload.
	void InitializeAllCollections();
	
	/**
	 * @brief Aggregates soft object paths from all soft object owners.
	 *
	 * Calls GetSoftObjectsToLoad on each owner retrieved by GetSoftObjectOwners
	 * and appends their results to the SoftObjectPaths array.
	 *
	 * @param SoftObjectPaths Reference to an array to store the aggregated soft object paths.
	 */
	virtual void GetSoftObjectsToLoad(const FGameplayTag& CollectionTag, TArray<FSoftObjectPath>& SoftObjectPaths) const;

	virtual void GetAlwaysLoadedSoftObjectsToLoad(TMap<FGameplayTag, TArray<FSoftObjectPath>>& SoftObjectPaths) const;

	virtual TSharedPtr<FStreamableHandle> LoadSoftObjects(const FGameplayTag& CollectionTag, const TArray<FSoftObjectPath>& SoftObjectPaths);

	static void CheckCollectionTag(const FGameplayTag& CollectionTag);

	void LoadAlwaysLoadedCollections();
};
