// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "NativeGameplayTags.h"
#include "DirtModal.generated.h"

class UTextBlock;
class UDirtButton;

DECLARE_DELEGATE_OneParam(FPromptDelegate, bool);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDynamicPromptDelegate, bool, bYes);

USERINTERFACE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataManagement_ID_ModalWidgetClass);

USTRUCT(BlueprintType)
struct USERINTERFACE_API FModalPayloadBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText TitleText;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText ModalText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText YesText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText NoText;

	FModalPayloadBase();
};

struct USERINTERFACE_API FModalPayload : public FModalPayloadBase
{
	FPromptDelegate Callback;
};

USTRUCT(BlueprintType)
struct USERINTERFACE_API FDynamicModalPayload : public FModalPayloadBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDynamicPromptDelegate Callback;
};

UCLASS(Abstract)
class USERINTERFACE_API UDirtModal : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:
#pragma region Properties
    
#pragma region UProperties
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ModalTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDirtButton> YesButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDirtButton> NoButton;

	UPROPERTY(EditAnywhere, Category = "Text")
	FText TitleText;

	UPROPERTY(EditAnywhere, Category = "Text")
	FText ModalText;
	
#pragma endregion UProperties

#pragma region Delegates
#pragma endregion Delegates

#pragma region Non-UProperties
#pragma endregion Non-UProperties

#pragma endregion Properties

public:
#pragma region PublicMethods
	template<typename TModal>
	static TModal* PushModal(const ULocalPlayer* LocalPlayer, const TSubclassOf<UDirtModal>& ModalClass, const FModalPayload& Payload)
	{
		return Cast<TModal>(PushModal(LocalPlayer, ModalClass, Payload));
	}
	
	static UDirtModal* PushModal(const ULocalPlayer* LocalPlayer, const TSubclassOf<UDirtModal>& ModalClass, const FModalPayload& Payload);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Push Modal"))
	static UDirtModal* K2_PushModal(const APlayerController* PlayerController, const TSubclassOf<UDirtModal>& ModalClass, const FDynamicModalPayload& Payload);
	
	UDirtModal();

#pragma region Setters
	UFUNCTION(BlueprintCallable)
	void SetTitleText(const FText& InTitleText);
	
	UFUNCTION(BlueprintCallable)
	void SetModalText(const FText& InPromptText);
	
	void SetModalInfo(const FModalPayload& Payload);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Modal Info"))
	void K2_SetModalInfo(const FDynamicModalPayload& Payload);
#pragma endregion Setters

#pragma region Getters
#pragma endregion Getters

#pragma region PublicParentInterfaces
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void NativePreConstruct() override;
	virtual void NativeOnActivated() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
#pragma endregion PublicParentInterfaces

#pragma endregion PublicMethods

protected:
#pragma region ProtectedMethods

#pragma region ProtectedParentInterfaces
#pragma endregion ProtectedParentInterfaces

#pragma endregion ProtectedMethods
};
