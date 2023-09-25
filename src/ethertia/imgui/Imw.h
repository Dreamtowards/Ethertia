//
// Created by Dreamtowards on 9/8/2023.
//

#include <ethertia/imgui/Imgui.h>

#include <ethertia/item/ItemStack.h>

#include <filesystem>

#include <ethertia/world/Entity.h>

#include "ImwGame.h"

class Imw
{
public:


    inline static ItemStack g_InventoryHoldingItemStack;

    static void ShowDockspaceAndMainMenubar();

    static void ItemImage(const Item* item, float size = 40, ImDrawList* dl = ImGui::GetWindowDrawList());

    static void ShowItemStack(ItemStack& stack, bool manipulation = false, float size = 40);


    struct Editor
    {

        static void ShowToolbar(bool* _open);


        inline static std::vector<std::string> ConsoleMessages;

        static void ShowConsole(bool* _open);

        
        inline static std::filesystem::path ExploringPath;

        static void ShowExplorer(bool* _open);



        // ImwProfiler.cpp
        static void ShowProfiler(bool* _open);


        static void ShowWorldGen(bool* _open);
    };

    class Outliner
    {

    };

    struct Gameplay
    {

        // ImwGameplay.cpp
    };



    // ImwSettings.cpp
    struct Settings
    {
        static void ShowSettings(bool* _open);

        inline static enum Panel {
            None,
            Profile,
            CurrentWorld,
            Graphics,
            Audio,
            Controls,
            Language,
            Mods,
            ResourcePacks,
            Credits,
            Misc
        } CurrentPanel = Panel::Graphics;
    };


    class Debug
    {
    public:

        static void ShowDialogs(bool* _open);
    };

};