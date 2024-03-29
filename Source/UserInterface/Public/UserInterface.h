﻿#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Modules/ModuleManager.h"

USERINTERFACE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Game);
USERINTERFACE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Menu);
USERINTERFACE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Layer_Modal);

class FUserInterfaceModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
