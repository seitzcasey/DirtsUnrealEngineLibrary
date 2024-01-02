// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Subsystems/EngineSubsystem.h"
#include "SoftObjectSubsystem.generated.h"

struct FStreamableHandle;

UCLASS(Abstract)
class DATAMANAGEMENT_API USoftObjectCollection : public UDataAsset
{
	GENERATED_BODY()
	
	friend class USoftObjectSubsystem;

	// Inherit from this and add your TSoftObjectPtr/TSoftObjectClass properties to be loaded.
	
protected:
	UPROPERTY(EditAnywhere)
	FName ID;
	
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
	virtual TArray<FSoftObjectPath> GetSoftObjectsToLoad() const;
};

/** Settings for the SoftObjectSubsystem, these settings are used manage collections of static and dynamic data sets to be loaded during runtime.*/
UCLASS(Config=Game, meta = (DisplayName = "Data Management"))
class DATAMANAGEMENT_API USoftObjectSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	friend class USoftObjectSubsystem;

protected:
	UPROPERTY(Config, EditAnywhere, Category = "Data Management")
	TArray<TSoftObjectPtr<USoftObjectCollection>> AlwaysLoadedCollections;
	
	UPROPERTY(Config, EditAnywhere, Category = "Data Management")
	TMap<FName, TSoftObjectPtr<USoftObjectCollection>> OnDemandCollections;

	static USoftObjectSettings* Get();
};

UCLASS()
class DATAMANAGEMENT_API USoftObjectSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
protected:
	FStreamableManager StreamableManager;
	
	// Map to store loaded collections and their objects
	TMap<FName, TSharedPtr<FStreamableHandle>> LoadedCollections;

	// Protect collections from GC.
	UPROPERTY()
	TArray<TObjectPtr<USoftObjectCollection>> AllCollections;
	
public:
	inline static FName AlwaysLoadedCollectionName = TEXT("AlwaysLoaded");
	
	/**
	 * @brief Retrieves the instance of USoftObjectSubsystem.
	 *
	 * Accesses the USoftObjectSubsystem instance from the global engine instance.
	 *
	 * @return USoftObjectSubsystem* Pointer to the USoftObjectSubsystem instance.
	 */
	static USoftObjectSubsystem* Get();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Method to load a specific collection of soft objects
	TSharedPtr<FStreamableHandle> LoadSoftObjectCollection(const FName CollectionName);

	// Method to unload a specific collection
	void UnloadSoftObjectCollection(const FName CollectionName);

	template<typename TSoftObjectCollection>
	TSoftObjectCollection* GetLoadedCollectionByID(const FName ID) const
	{
		return Cast<TSoftObjectCollection>(K2_GetLoadedCollectionByID(ID));
	}

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Loaded Collection By ID"))
	USoftObjectCollection* K2_GetLoadedCollectionByID(const FName ID) const;

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
	virtual void GetSoftObjectsToLoad(TArray<FSoftObjectPath>& SoftObjectPaths, const FName CollectionName) const;

	virtual void GetAlwaysLoadedSoftObjectsToLoad(TArray<FSoftObjectPath>& SoftObjectPaths) const;

	virtual TSharedPtr<FStreamableHandle> LoadSoftObjects(const TArray<FSoftObjectPath>& SoftObjectPaths, const FName CollectionName = NAME_None);
};
