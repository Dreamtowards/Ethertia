//
// Created by Dreamtowards on 9/8/2023.
//

#include <ethertia/imgui/Imgui.h>

#include <ethertia/item/ItemStack.h>

#include <filesystem>

#include <ethertia/world/Entity.h>

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
            eWorld,
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

        inline static std::unordered_map<uint32_t, std::function<void(void*)>> ComponentInspectors;

        template<typename ComponentType>
        static void AddComponentInspector(const std::function<void(ComponentType&)>& fn)
        {
            ComponentInspectors[entt::type_hash<ComponentType>().value()] = [=](void* cp_data) 
                {
                    fn(*static_cast<ComponentType*>(cp_data));
                };
        }

        inline static Entity SelectedEntity{};

        static void ShowHierarchy(bool* _open);


        // ImwProfiler.cpp
        static void ShowProfiler(bool* _open);


        static void ShowWorldGen(bool* _open);
    };

    class Outliner
    {

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