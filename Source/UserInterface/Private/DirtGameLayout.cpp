// Fill out your copyright notice in the Description page of Project Settings.

#include "DirtGameLayout.h"
#include "UserInterface.h"

void UDirtGameLayout::NativePreConstruct()
{
	Super::NativePreConstruct();

	RegisterLayer(UI_Layer_Game, GameStack);
	RegisterLayer(UI_Layer_Menu, MenuStack);
	RegisterLayer(UI_Layer_Modal, ModalStack);
}
