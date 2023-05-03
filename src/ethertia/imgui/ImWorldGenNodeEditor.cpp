//
// Created by Thaumstrial on 2023-05-03.
//
#include <glad/glad.h>
#include "ImWorldGenNodeEditor.h"
#include "imgui_internal.h"
#include <imgui-imnodes/imnodes.h>
#include <cstring>

using namespace ImWorldGenNodeEditor;

template<typename... Args>
std::string string_format( const char* format, Args... args )
{
    int size_s = std::snprintf( nullptr, 0, "%s", format, args... );
    if( size_s <= 0 )
    {
        return "";
    }
    auto size = static_cast<size_t>( size_s );
    std::string buf( size, 0 );
    std::snprintf( buf.data(), size, "%s", format, args... );
    return buf;
}

template<typename... T>
static bool DoHoverPopup( const char* format, T... args )
{
    if( ImGui::IsItemHovered() )
    {
        std::string hoverTxt = string_format( format, args... );

        if( hoverTxt.empty() )
        {
            return false;
        }

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4.f, 4.f ) );
        ImGui::BeginTooltip();
        ImGui::TextUnformatted( hoverTxt.c_str() );
        ImGui::EndTooltip();
        ImGui::PopStyleVar();
        return true;
    }
    return false;
}

void WorldGenNodeEditor::ShowWorldGenNodeEditor(bool& w_WorldGenNodeEditor) {
    ImGui::Begin("WorldGenNodeEdit", &w_WorldGenNodeEditor);
    ImNodes::BeginNodeEditor();

    // Add nodes
    if (ImGui::IsWindowFocused() && ImNodes::IsEditorHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup("Add Node");
    }
    if (ImGui::BeginPopup("Add Node"))
    {
        mContextStartPos = ImGui::GetMousePosOnOpeningCurrentPopup();
        if(auto newMetadata = mContextMetadata.front()->DrawUI())
        {
            AddNode(mContextStartPos, newMetadata);
        }
        ImGui::EndPopup();
    }

    // Render nodes
    for(auto& node : mNodes)
    {
        ImNodes::BeginNode(node.second.nodeId);

        ImNodes::BeginNodeTitleBar();
        std::string formatName = FastNoise::Metadata::FormatMetadataNodeName(node.second.data->metadata);
        ImGui::TextUnformatted(formatName.c_str());
        ImNodes::EndNodeTitleBar();

        ImGui::PushItemWidth(60.0f);

        ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkCreationOnSnap);
        ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
        int attributeId = node.second.GetStartingAttributeId();
        auto& nodeMetadata = node.second.data->metadata;
        auto& nodeData = node.second.data;

        for(auto& memberNode : nodeMetadata->memberNodeLookups)
        {
            ImNodes::BeginInputAttribute(attributeId++);
            formatName = FastNoise::Metadata::FormatMetadataMemberName(memberNode);
            ImGui::TextUnformatted(formatName.c_str());
            ImNodes::EndInputAttribute();

            DoHoverPopup(memberNode.description);
        }

        for(size_t i = 0; i < node.second.data->metadata->memberHybrids.size(); i++)
        {
            ImNodes::BeginInputAttribute(attributeId++);

            bool isLinked = node.second.data->hybrids[i].first;
            const char* floatFormat = "%.3f";

            if(isLinked)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                floatFormat = "";
            }

            formatName = FastNoise::Metadata::FormatMetadataMemberName(nodeMetadata->memberHybrids[i]);

            if(ImGui::DragFloat(formatName.c_str(), &nodeData->hybrids[i].second, 0.02f, 0, 0, floatFormat))
            {
                node.second.GeneratePreview();
            }

            if(isLinked)
            {
                ImGui::PopItemFlag();
            }
            ImNodes::EndInputAttribute();
            DoHoverPopup(nodeMetadata->memberHybrids[i].description);
        }

        for(size_t i = 0; i < nodeMetadata->memberVariables.size(); i++)
        {
            ImNodes::BeginStaticAttribute(0);

            auto& nodeVar = nodeMetadata->memberVariables[i];

            formatName = FastNoise::Metadata::FormatMetadataMemberName(nodeVar);

            switch(nodeVar.type)
            {
                case FastNoise::Metadata::MemberVariable::EFloat:
                {
                    if( ImGui::DragFloat( formatName.c_str(), &nodeData->variables[i].f, 0.02f, nodeVar.valueMin.f, nodeVar.valueMax.f ) )
                    {
                        node.second.GeneratePreview();
                    }
                }
                    break;
                case FastNoise::Metadata::MemberVariable::EInt:
                {
                    if(ImGui::DragInt( formatName.c_str(), &nodeData->variables[i].i, 0.2f, nodeVar.valueMin.i, nodeVar.valueMax.i ))
                    {
                        node.second.GeneratePreview();
                    }
                }
                    break;
                case FastNoise::Metadata::MemberVariable::EEnum:
                {
                    if(ImGui::Combo( formatName.c_str(), &nodeData->variables[i].i, nodeVar.enumNames.data(), (int)nodeVar.enumNames.size() ) ||
                        ImGuiExtra::ScrollCombo( &nodeData->variables[i].i, (int)nodeVar.enumNames.size()))
                    {
                        node.second.GeneratePreview();
                    }
                }
                    break;
            }

            ImNodes::EndStaticAttribute();
            DoHoverPopup( nodeMetadata->memberVariables[i].description );
        }

        ImGui::PopItemWidth();
        ImNodes::PopAttributeFlag();
        ImNodes::BeginOutputAttribute( node.second.GetOutputAttributeId(), ImNodesPinShape_QuadFilled );

//        if( mSelectedNode == node.first && !node.second.serialised.empty() )
//        {
//            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
//            ImGui::RenderFrame( cursorPos - ImVec2( 1, 1 ), cursorPos + ImVec2( noiseSize ) + ImVec2( 1, 1 ), IM_COL32( 255, 0, 0, 200 ), false );
//        }

        ImGui::Image((void*)(intptr_t)(node.second.noiseTexture), ImVec2((float)Node::NoiseSize, (float)Node::NoiseSize));

        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();
    }
    // Render links
    for(auto& node : mNodes)
    {
        int attributeId = node.second.GetStartingAttributeId();

        for(FastNoise::NodeData* link : node.second.GetNodeIDLinks())
        {
            if(link)
            {
                ImNodes::Link(attributeId, mNodes.at(link).GetOutputAttributeId(), attributeId);
            }
            attributeId++;
        }
    }

    ImNodes::EndNodeEditor();

    // Check links
    int startNodeId, endNodeId;
    int startAttr, endAttr;
    bool createdFromSnap;
    if( ImNodes::IsLinkCreated( &startNodeId, &startAttr, &endNodeId, &endAttr, &createdFromSnap ) )
    {
        Node* startNode = FindNodeFromId( startNodeId );
        Node* endNode = FindNodeFromId( endNodeId );

        if( startNode && endNode )
        {
            auto& link = endNode->GetNodeLink( endAttr );

            if( !createdFromSnap || !link )
            {
                link = startNode->data.get();
                endNode->GeneratePreview();
            }
        }
    }

    // Remove nodes or links
    std::vector<int> linksToDelete;
    int selectedLinkCount = ImNodes::NumSelectedLinks();

    bool delKeyPressed =
            ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_Delete ), false ) ||
            ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_Backspace ), false );

    if( selectedLinkCount && delKeyPressed )
    {
        linksToDelete.resize( selectedLinkCount );
        ImNodes::GetSelectedLinks( linksToDelete.data() );
    }

    int destroyedLinkId;
    if( ImNodes::IsLinkDestroyed( &destroyedLinkId ) )
    {
        linksToDelete.push_back( destroyedLinkId );
    }

    for( int deleteID : linksToDelete )
    {
        for( auto& node : mNodes )
        {
            bool changed = false;
            int attributeId = node.second.GetStartingAttributeId();

            for( FastNoise::NodeData* link : node.second.GetNodeIDLinks() )
            {
                (void)link;
                if( attributeId == deleteID )
                {
                    node.second.GetNodeLink( attributeId ) = nullptr;
                    changed = true;
                }
                attributeId++;
            }

            if( changed )
            {
                node.second.GeneratePreview();
            }
        }
    }

    int selectedNodeCount = ImNodes::NumSelectedNodes();

    if( selectedNodeCount && ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_Delete ), false ) )
    {
        std::vector<int> selected( selectedNodeCount );

        ImNodes::GetSelectedNodes( selected.data() );

        for( int deleteID: selected )
        {
            if( Node* node = FindNodeFromId( deleteID ) )
            {
                FastNoise::NodeData* nodeData = node->data.get();
                mNodes.erase( nodeData );

                for( auto& targetNode : mNodes )
                {
                    bool changed = false;
                    int attrId = targetNode.second.GetStartingAttributeId();

                    for( FastNoise::NodeData* link : targetNode.second.GetNodeIDLinks() )
                    {
                        if( link == nodeData )
                        {
                            targetNode.second.GetNodeLink( attrId ) = nullptr;
                            changed = true;
                        }
                        attrId++;
                    }

                    if( changed )
                    {
                        targetNode.second.GeneratePreview();
                    }
                }
            }
        }
    }

    int attrStart;
    if( ImNodes::IsLinkDropped( &attrStart, false ) )
    {
        Node* node = FindNodeFromId( Node::GetNodeIdFromAttribute( attrStart ) );

        if( node && ( attrStart & Node::AttributeBitMask ) == Node::AttributeBitMask )
        {
            mDroppedLinkNode = node->data.get();
            mDroppedLink = true;
        }
    }
    ImGui::End();
}

WorldGenNodeEditor::WorldGenNodeEditor() {

    std::unordered_map<std::string, MetadataMenuGroup*> groupTree2MenuGroup;
    auto* root = new MetadataMenuGroup("");
    mContextMetadata.emplace_back(root);

    auto menuSort = [](const MetadataMenu* a, const MetadataMenu* b) {return std::strcmp( a->GetName(), b->GetName() ) < 0;};

    for(const FastNoise::Metadata* metadata : FastNoise::Metadata::GetAll())
    {
        auto* metaDataGroup = root;
        std::string groupTree;

        for(const char* group : metadata->groups)
        {
            groupTree += group;
            auto find = groupTree2MenuGroup.find(groupTree);
            if( find == groupTree2MenuGroup.end())
            {
                auto* newGroup = new MetadataMenuGroup(group);
                mContextMetadata.emplace_back(newGroup);
                metaDataGroup->items.emplace_back(newGroup);
                find = groupTree2MenuGroup.emplace(groupTree, newGroup).first;
                std::sort(metaDataGroup->items.begin(), metaDataGroup->items.end(), menuSort);
            }
            metaDataGroup = find->second;
            groupTree += '\t';
        }

        metaDataGroup->items.emplace_back( mContextMetadata.emplace_back(new MetadataMenuItem(metadata)).get());
        std::sort(metaDataGroup->items.begin(), metaDataGroup->items.end(), menuSort);
    }
}

const FastNoise::Metadata* WorldGenNodeEditor::MetadataMenuItem::DrawUI(std::function<bool( const FastNoise::Metadata* )> isValid, bool drawGroups) const
{
    std::string format = FastNoise::Metadata::FormatMetadataNodeName(metadata, true);

    if( ImGui::MenuItem(format.c_str()))
    {
        return metadata;
    }
    return nullptr;
}

const FastNoise::Metadata* WorldGenNodeEditor::MetadataMenuGroup::DrawUI(std::function<bool( const FastNoise::Metadata* )> isValid, bool drawGroups) const
{
    const FastNoise::Metadata* returnPressed = nullptr;

    bool doGroup = drawGroups && name[0] != 0;

    if( !doGroup || ImGui::BeginMenu(name))
    {
        for(const auto& item : items)
        {
            if(item->CanDraw(isValid))
            {
                if( auto pressed = item->DrawUI(isValid, drawGroups))
                {
                    returnPressed = pressed;
                }
            }
        }
        if(doGroup)
        {
            ImGui::EndMenu();
        }
    }
    return returnPressed;
}

bool WorldGenNodeEditor::MetadataMenuItem::CanDraw( std::function<bool( const FastNoise::Metadata* )> isValid ) const
{
    return !isValid || isValid( metadata );
}

bool WorldGenNodeEditor::MetadataMenuGroup::CanDraw( std::function<bool( const FastNoise::Metadata* )> isValid ) const
{
    for( const auto& item : items )
    {
        if( item->CanDraw( isValid ) )
        {
            return true;
        }
    }
    return false;
}

WorldGenNodeEditor::Node& WorldGenNodeEditor::AddNode(ImVec2 startPos, const FastNoise::Metadata* metadata, bool generatePreview)
{
    auto* nodeData = new FastNoise::NodeData(metadata);

    auto newNode = mNodes.try_emplace(nodeData, *this, nodeData, generatePreview);

    ImNodes::SetNodeScreenSpacePos(newNode.first->second.nodeId, startPos);

    return newNode.first->second;
}

WorldGenNodeEditor::Node::Node(WorldGenNodeEditor& e, FastNoise::NodeData* nodeData, bool generatePreview, int id) :
        Node(e, std::unique_ptr<FastNoise::NodeData>( nodeData ), generatePreview, id)
{ }

WorldGenNodeEditor::Node::Node(WorldGenNodeEditor& e, std::unique_ptr<FastNoise::NodeData>&& nodeData, bool generatePreview, int id) :
        editor(e),
        data(std::move( nodeData)),
        nodeId(id ? id : e.GetFreeNodeId())
{
    glGenTextures(1, &noiseTexture);
    assert(!e.FindNodeFromId(id));
    if(generatePreview)
    {
        GeneratePreview();
    }
}

std::vector<FastNoise::NodeData*> WorldGenNodeEditor::Node::GetNodeIDLinks()
{
    std::vector<FastNoise::NodeData*> links;
    links.reserve( data->nodeLookups.size() + data->hybrids.size() );

    for( FastNoise::NodeData* link : data->nodeLookups )
    {
        links.emplace_back( link );
    }

    for( auto& link : data->hybrids )
    {
        links.emplace_back( link.first );
    }

    assert( links.size() < 16 );

    return links;
}

WorldGenNodeEditor::Node* WorldGenNodeEditor::FindNodeFromId(int id)
{
    auto find = std::find_if( mNodes.begin(), mNodes.end(), [id]( const auto& node )
    {
        return node.second.nodeId == id;
    } );

    if( find != mNodes.end() )
    {
        return &find->second;
    }

    return nullptr;
}

int WorldGenNodeEditor::GetFreeNodeId()
{
    static int newNodeId = 0;

    do
    {
        newNodeId = std::max( 1, ( newNodeId + 1 ) & ( INT_MAX >> Node::AttributeBitCount ) );

    } while( FindNodeFromId( newNodeId ) );

    return newNodeId;
}

void WorldGenNodeEditor::Node::GeneratePreview(bool nodeTreeChanged)
{
    static float noiseData[NoiseSize * NoiseSize];
    static std::array<uint8_t, NoiseSize * NoiseSize> imageData;

    serialised = FastNoise::Metadata::SerialiseNodeData( data.get(), true );
    auto generator = FastNoise::NewFromEncodedNodeTree( serialised.c_str(), WorldGenNodeEditor::mActualSIMDLevel );

    if( generator )
    {
        auto genRGB = FastNoise::New<FastNoise::ConvertRGBA8>(WorldGenNodeEditor::mActualSIMDLevel);
        genRGB->SetSource(generator);
        genRGB->GenUniformGrid2D(noiseData,
                                0, 0,
                                Node::NoiseSize, Node::NoiseSize,
                                mNodeFrequency, mNodeSeed );
    }
    else
    {
        std::fill(std::begin(noiseData), std::end(noiseData), 0.0f);
        serialised.clear();
    }


    for (int i = 0; i < NoiseSize * NoiseSize; ++i) {
        float value = (noiseData[i] + 1.0f) / 2.0f;
        imageData[i] =  static_cast<uint8_t>(value * 255.0f);
    }

    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, NoiseSize, NoiseSize, 0, GL_RED, GL_UNSIGNED_BYTE, imageData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    ImageView2D noiseImage( PixelFormat::RGBA8Unorm, { NoiseSize, NoiseSize }, noiseData );

//    noiseTexture.setStorage( 1, GL::TextureFormat::RGBA8, noiseImage.size() ).setSubImage( 0, {}, noiseImage );

    for( auto& node : editor.mNodes )
    {
        for( FastNoise::NodeData* link : node.second.GetNodeIDLinks() )
        {
            if( link == data.get() )
            {
                node.second.GeneratePreview( nodeTreeChanged );
            }
        }
    }
    
}

FastNoise::NodeData*& WorldGenNodeEditor::Node::GetNodeLink( int attributeId )
{
    attributeId &= 15;

    if( attributeId < (int)data->nodeLookups.size() )
    {
        return data->nodeLookups[attributeId];
    }
    else
    {
        attributeId -= (int)data->nodeLookups.size();
        return data->hybrids[attributeId].first;
    }
}