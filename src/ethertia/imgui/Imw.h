//
// Created by Dreamtowards on 9/8/2023.
//

#include <ethertia/imgui/Imgui.h>

#include <ethertia/item/ItemStack.h>

#include <filesystem>

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


        enum InspectionType
        {
            eNone,
            eEntity,
            eCamera,
            ePipeline
        };

        inline static void* _InspectionObject = nullptr;
        inline static InspectionType _InspectionType;

        static void SetInspectionObject(void* inspecting, InspectionType type)
        {
            _InspectionObject = inspecting;
            _InspectionType = type;
        }

        // ImwInspector.cpp
        static void ShowInspector(bool* _open);

        static void ShowHierarchy(bool* _open);


        // ImwProfiler.cpp
        static void ShowProfiler(bool* _open);


        static void ShowWorldGen(bool* _open);
    };

    struct Gameplay
    {
        inline static bool GameFullwindow = false;

        inline static VkImageView GameImageView = nullptr;

        static void ShowGame(bool* _open);


        // ImwGameplay.cpp
        static void ShowWorldNew(bool* _open);

        static void ShowWorldList(bool* _open);

        static void ShowWorldModify(bool* _open);

        static void ShowTitleScreen(bool* _open);
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