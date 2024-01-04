// Fill out your copyright notice in the Description page of Project Settings.

#include "DirtModal.h"
#include "CommonUIExtensions.h"
#include "DirtButton.h"
#include "UserInterface.h"
#include "Components/TextBlock.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(DataManagement_ID_ModalWidgetClass, "DataManagement.ID.ModalWidgetClass", "Object ID for the UI");

FModalPayloadBase::FModalPayloadBase()
	: YesText(NSLOCTEXT("Prompt", "Yes", "Yes"))
	, NoText(NSLOCTEXT("Prompt", "No", "No"))
{
}

UDirtModal* UDirtModal::PushModal(const ULocalPlayer* LocalPlayer, const TSubclassOf<UDirtModal>& ModalClass,
	const FModalPayload& Payload)
{
	auto* Modal = Cast<UDirtModal>(UCommonUIExtensions::PushContentToLayer_ForPlayer(LocalPlayer, UI_Layer_Modal, ModalClass));
	if ensureAlways(Modal)
	{
		Modal->SetModalInfo(Payload);
	}
	return Modal;
}

UDirtModal* UDirtModal::K2_PushModal(const APlayerController* PlayerController, const TSubclassOf<UDirtModal>& ModalClass,
	const FDynamicModalPayload& Payload)
{
	auto const* LocalPlayer = PlayerController ? Cast<ULocalPlayer>(PlayerController->Player) : nullptr;
	auto* Modal = Cast<UDirtModal>(UCommonUIExtensions::PushContentToLayer_ForPlayer(LocalPlayer, UI_Layer_Modal, ModalClass));
	if ensureAlways(Modal)
	{
		Modal->K2_SetModalInfo(Payload);
	}
	return Modal;
}

UDirtModal::UDirtModal()
{
}

void UDirtModal::SetTitleText(const FText& InTitleText)
{
	TitleText = InTitleText;

	if ensureAlways(IsValid(TitleTextBlock))
	{
		TitleTextBlock->SetText(TitleText);
	}
}

void UDirtModal::SetModalText(const FText& InModalText)
{
	ModalText = InModalText;

	if ensureAlways(IsValid(ModalTextBlock))
	{
		ModalTextBlock->SetText(ModalText);
	}
}

void UDirtModal::SetModalInfo(const FModalPayload& Payload)
{
	SetModalText(Payload.ModalText);

	if ensureAlways(IsValid(YesButton))
	{
		YesButton->OnClicked().AddWeakLambda(this, [Callback = Payload.Callback]()
		{
			Callback.ExecuteIfBound(true);
		});
	}

	if ensureAlways(IsValid(NoButton))
	{
		NoButton->OnClicked().AddWeakLambda(this, [this, Callback = Payload.Callback]()
		{
			Callback.ExecuteIfBound(false);

			// Close the modal
			UCommonUIExtensions::PopContentFromLayer(this);
		});
	}
}

void UDirtModal::K2_SetModalInfo(const FDynamicModalPayload& Payload)
{
	SetModalText(Payload.ModalText);

	if ensureAlways(IsValid(YesButton))
	{
		YesButton->SetText(Payload.YesText);
		
		YesButton->OnClicked().AddWeakLambda(this, [Callback = Payload.Callback]()
		{
			Callback.ExecuteIfBound(true);
		});
	}

	if ensureAlways(IsValid(NoButton))
	{
		NoButton->SetText(Payload.NoText);
		
		NoButton->OnClicked().AddWeakLambda(this, [this, Callback = Payload.Callback]()
		{
			Callback.ExecuteIfBound(false);

			// Close the modal
			UCommonUIExtensions::PopContentFromLayer(this);
		});
	}
}

void UDirtModal::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UDirtModal::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

void UDirtModal::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (!TitleText.IsEmpty())
	{
		SetTitleText(TitleText);	
	}
	else if (IsValid(TitleTextBlock))
	{
		TitleText = TitleTextBlock->GetText();
	}
	
	if (!ModalText.IsEmpty())
	{
		SetModalText(ModalText);	
	}
	else if (IsValid(ModalTextBlock))
	{
		ModalText = ModalTextBlock->GetText();
	}
}

void UDirtModal::NativeOnActivated()
{
	if (auto* FocusTarget = GetDesiredFocusTarget())
	{
		FocusTarget->SetFocus();
	}
}

UWidget* UDirtModal::NativeGetDesiredFocusTarget() const
{
	return YesButton;
}
