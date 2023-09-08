//
// Created by Dreamtowards on 9/8/2023.
//

#include <ethertia/imgui/Imgui.h>


class Imw
{
public:

    static void InitWindows();



    inline static ItemStack g_InventoryHoldingItemStack;

    static void ShowDockspaceAndMainMenubar();

    static void ItemImage(const Item* item, float size = 40, ImDrawList* dl = ImGui::GetWindowDrawList());

    static void ShowItemStack(ItemStack& stack, bool manipulation = false, float size = 40);




    static void ShowConsole(bool* _open);

    static void ShowToolbar(bool* _open);


    // ImwProfiler.cpp
    static void ShowProfiler(bool* _open);

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


    // ImwInspector.cpp
    static void ShowInspector(bool* _open);

    static void ShowEntityList(bool* _open);



    // ImwGameplay.cpp
    static void ShowWorldNew(bool* _open);
};