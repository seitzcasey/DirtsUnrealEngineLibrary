#include "UserInterface.h"

#define LOCTEXT_NAMESPACE "FUserInterfaceModule"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Game, "UI.Layer.Game", "Game layer. Widgets related to gameplay.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Menu, "UI.Layer.Menu", "Menu Layer. Widgets related to menus. Main Menu, Escape Menu, etc.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Modal, "UI.Layer.Modal", "Modal Layer. Widgets related to pop-ups.");

void FUserInterfaceModule::StartupModule()
{
    
}

void FUserInterfaceModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUserInterfaceModule, UserInterface)