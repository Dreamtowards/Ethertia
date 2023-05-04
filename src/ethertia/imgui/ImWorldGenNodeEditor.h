//
// Created by Thaumstrial on 2023-05-03.
//

#ifndef ETHERTIA_IMWORLDGENNODEEDITOR_H
#define ETHERTIA_IMWORLDGENNODEEDITOR_H

#include <FastSIMD/FastSIMD.h>
#include <FastNoise/FastNoise.h>
#include <FastNoise/Metadata.h>
#include <imgui_internal.h>
#include <imgui.h>

#include <unordered_map>
#include <array>

// if need include OpenGL, better try include GLFW or glad which is cross-platform. (GL/gl.h is not supported on Mac computers at default.)
// #include <GL/gl.h>

namespace ImGuiExtra
{
    inline void MarkSettingsDirty()
    {
        if( ImGui::GetCurrentContext() && ImGui::GetCurrentContext()->SettingsDirtyTimer <= 0.0f )
        {
            ImGui::GetCurrentContext()->SettingsDirtyTimer = ImGui::GetIO().IniSavingRate;
        }
    }

    inline void AddOrReplaceSettingsHandler( ImGuiSettingsHandler& settings )
    {
        if( auto* existing = ImGui::FindSettingsHandler( settings.TypeName ) )
        {
            *existing = settings;
        }
        else
        {
            ImGui::GetCurrentContext()->SettingsHandlers.push_back( settings );
        }
    }

    inline bool ScrollCombo( int* comboIndex, int comboCount )
    {
        if( ImGui::IsItemHovered() )
        {
            ImGui::SetItemUsingMouseWheel();

            if( ImGui::GetIO().MouseWheel < 0 && *comboIndex < comboCount - 1 )
            {
                (*comboIndex)++;
                return true;
            }

            if( ImGui::GetIO().MouseWheel > 0 && *comboIndex > 0 )
            {
                (*comboIndex)--;
                return true;
            }
        }
        return false;
    }
}

namespace ImWorldGenNodeEditor
{
    class WorldGenNodeEditor
    {
    public:
        WorldGenNodeEditor();
        void ShowWorldGenNodeEditor(bool& w_WorldGenNodeEditor);
        inline static FastSIMD::eLevel mActualSIMDLevel = FastSIMD::Level_Null;

    private:
        struct Node
        {
            Node(WorldGenNodeEditor& editor, FastNoise::NodeData* nodeData, bool generatePreview = true, int id = 0);
            Node(WorldGenNodeEditor& e, std::unique_ptr<FastNoise::NodeData>&& nodeData, bool generatePreview, int id);
            ~Node()
            {
                glDeleteTextures(1, &noiseTexture);
            }
            void GeneratePreview(bool nodeTreeChanged = true);
            FastNoise::NodeData*& GetNodeLink( int attributeId );

            std::vector<FastNoise::NodeData*> GetNodeIDLinks();
            static constexpr int AttributeBitCount = 8;
            static constexpr int AttributeBitMask = ( 1 << AttributeBitCount ) - 1;

            static int GetNodeIdFromAttribute( int attributeId )
            {
                return (int)((unsigned int)attributeId >> AttributeBitCount);
            }

            int GetStartingAttributeId() const
            {
                return nodeId << AttributeBitCount;
            }

            int GetOutputAttributeId() const
            {
                return GetStartingAttributeId() | AttributeBitMask;
            }

            WorldGenNodeEditor& editor;
            std::unique_ptr<FastNoise::NodeData> data;
            std::string serialised;
            const int nodeId;
            static const int NoiseSize = 256;
            GLuint noiseTexture;
            float mNodeFrequency = 1 / 100.0f;
            int mNodeSeed = 1337;

        };

        struct MetadataMenu
        {
            virtual ~MetadataMenu() = default;
            virtual const char* GetName() const = 0;
            virtual bool CanDraw( std::function<bool( const FastNoise::Metadata* )> isValid = nullptr ) const = 0;
            virtual const FastNoise::Metadata* DrawUI( std::function<bool( const FastNoise::Metadata* )> isValid = nullptr, bool drawGroups = true ) const = 0;
        };

        struct MetadataMenuItem : MetadataMenu
        {
            MetadataMenuItem( const FastNoise::Metadata* metadata ) : metadata( metadata ) {}

            const char* GetName() const final { return metadata->name; }
            bool CanDraw( std::function<bool( const FastNoise::Metadata* )> isValid ) const final;
            const FastNoise::Metadata* DrawUI( std::function<bool( const FastNoise::Metadata* )> isValid, bool drawGroups ) const final;

            const FastNoise::Metadata* metadata;
        };

        struct MetadataMenuGroup : MetadataMenu
        {
            MetadataMenuGroup( const char* name ) : name( name ) {}

            const char* GetName() const final { return name; }
            bool CanDraw( std::function<bool( const FastNoise::Metadata* )> isValid ) const final;
            const FastNoise::Metadata* DrawUI( std::function<bool( const FastNoise::Metadata* )> isValid, bool drawGroups ) const final;

            const char* name;
            std::vector<const MetadataMenu*> items;
        };

        Node& AddNode( ImVec2 startPos, const FastNoise::Metadata* metadata, bool generatePreview = true );
        bool AddNodeFromEncodedString( const char* string, ImVec2 nodePos );
        Node* FindNodeFromId( int id );
        int GetFreeNodeId();
        void SetupSettingsHandlers();

        void DoHelp();

        std::unordered_map<FastNoise::NodeData*, Node> mNodes;
        FastNoise::NodeData* mDroppedLinkNode = nullptr;
        bool mDroppedLink = false;

        ImVec2 mContextStartPos;
        std::vector<std::unique_ptr<MetadataMenu>> mContextMetadata;
        std::string mImportNodeString;
        bool mImportNodeModal = false;

        FastNoise::NodeData* mSelectedNode = nullptr;

        FastSIMD::eLevel mMaxSIMDLevel = FastSIMD::CPUMaxSIMDLevel();
    };

}

#endif //ETHERTIA_IMWORLDGENNODEEDITOR_H
