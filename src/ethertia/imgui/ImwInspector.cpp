//
// Created by Dreamtowards on 2023/5/14.
//

#include "ImwInspector.h"
#include <ethertia/imgui/Imgui.h>

#include <format>
#include <glm/gtx/string_cast.hpp>

#include <ethertia/Ethertia.h>
#include <ethertia/init/Settings.h>
#include <ethertia/init/DebugStat.h>

#include <ethertia/world/World.h>
#include <ethertia/world/Entity.h>



#pragma region Hierarchy


static void _ShowCreateEntityMenu(World* world)
{
    if (ImGui::MenuItem("Entity"))
    {
        world->CreateEntity();
    }
    if (ImGui::BeginMenu("Geometry"))
    {
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Light"))
    {
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Visual Effects"))
    {
        ImGui::EndMenu();
    }
    ImGui::Separator();
    if (ImGui::BeginMenu("Vehicle"))
    {
        ImGui::EndMenu();
    }
}



void ImwInspector::ShowHierarchy(bool* _open)
{
    ImGui::Begin("World Hierarchy", _open);

    World* world = Ethertia::GetWorld();
    if (!world) {
        ImGui::TextDisabled("World not loaded.");
        ImGui::End();
        return;
    }

    ImGui::Button("+");
    ImGui::SameLine();
    if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonLeft))
    {
        _ShowCreateEntityMenu(world);
        ImGui::EndPopup();
    }

    ImGui::ArrowButton("##filter", ImGuiDir_Down);
    ImGui::SameLine();

    if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonLeft))
    {
        bool bol;
        int s_ListMaxCount = 0;
        ImGui::SeparatorText("Filter");
        ImGui::Checkbox("Exclude OutOfFrustum Entities", &bol);
        // Sort by distance?
        ImGui::DragInt("List Max Number", &s_ListMaxCount);
        // KeepSelect HitEntity
        // IgnoreChunk Entities

        ImGui::EndPopup();
    }

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    static char _Search[128];
    ImGui::InputTextWithHint("##search", "Find..", _Search, std::size(_Search));
    ImGui::PopItemWidth();

    ImGui::Separator();

    ImGui::BeginChild("entitylist", { 0, -ImGui::GetTextLineHeightWithSpacing() });

    if (ImGui::BeginTable("table", 2,
        ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_SizingStretchProp, {}))
    {
        //ImGui::TableSetupColumn("##0", ImGuiTableColumnFlags_WidthFixed, 16);
        //ImGui::TableSetupColumn("##Name", ImGuiTableColumnFlags_WidthStretch, 200);
        //ImGui::TableSetupColumn("##Type", ImGuiTableColumnFlags_WidthFixed, 80);
        //ImGui::TableHeadersRow();

        world->registry().each([&](entt::entity eid) 
            {
                Entity entity = { eid, world };
                auto& tag = entity.GetComponent<TagComponent>();

                ImGui::PushID(entity.id());
                ImGui::TableNextRow();

                ImGui::TableNextColumn();

                if (ImGui::Selectable("##sel", SelectedEntity == entity, ImGuiSelectableFlags_SpanAllColumns))
                {
                    SelectedEntity = SelectedEntity == entity ? Entity{} : entity;
                }
                Imgui::ItemTooltip(std::format("entt_id: {}", entity.id()));

                ImGui::SameLine();

                if (tag.IsEnabled) {
                    ImGui::Text("%s", tag.Name.c_str());
                }
                else {
                    ImGui::TextDisabled("%s", tag.Name.c_str());
                }

                ImGui::TableNextColumn();

                ImGui::TextDisabled("type");

                ImGui::PopID();
            });

        ImGui::EndTable();
    }

    if (SelectedEntity)
    {
        if (ImGui::BeginPopupContextItem("MenuOpEntity"))
        {
            //ImGui::TextDisabled("");
            if (ImGui::MenuItem("Destroy Entity")) {
                world->DestroyEntity(SelectedEntity);
            }

            ImGui::EndPopup();
        }
    }

    ImGui::EndChild();


    ImGui::TextDisabled(std::format("{} entity.", world->registry().size()).c_str());



    //if (ImGui::Selectable("Main Camera")) {
    //    Imw::Editor::SetInspectionObject(&Ethertia::getCamera(), eCamera);
    //}
    //
    //if (ImGui::Selectable("Pipeline: GBuffer")) {
    //    Imw::Editor::SetInspectionObject(&Ethertia::getCamera(), eCamera);
    //}


    ImGui::End();
}





#pragma endregion






#pragma region Imw Inspector


void ImwInspector::ShowInspector(bool* _open)
{
    ImGui::Begin("Inspector", _open);

    Entity entity = SelectedEntity;
    if (!entity) {
        ImGui::TextDisabled("Select an entity to inspect.");
        ImGui::End();
        return;
    }

    ImGui::Button("+");
    ImGui::SameLine();

    if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonLeft))
    {
        if (ImGui::MenuItem("std::uint16_t"))
        {
            entity.AddComponent<std::uint16_t>();
        }
        if (ImGui::MenuItem("std::string"))
        {
            entity.AddComponent<std::string>();
        }
        if (ImGui::MenuItem("DebugDrawAABB"))
        {
            entity.AddComponent<DebugDrawBoundingBox>();
        }

        ImGui::EndPopup();
    }
    
    auto& tag = entity.GetComponent<TagComponent>();

    ImGui::Checkbox("##IsEnabled", &tag.IsEnabled);
    Imgui::ItemTooltip("Enabled or Disable");

    ImGui::SameLine();

    Imgui::InputText("##Name", tag.Name, "Entity Name");
    Imgui::ItemTooltip("Entity Name, is not unique, it's kinda trivial");
    

    ImGui::Separator();

    ImGui::BeginChild("ComponentsList", { 0, -ImGui::GetTextLineHeightWithSpacing() });

    int numEntityComponents = 0;
    
    // for Pools of ComponentType
    for (const auto& it : entity.reg().storage())
    {
        if (auto& c_storage = it.second; c_storage.contains(entity))
        {
            ++numEntityComponents;
            auto& c_type = c_storage.type();  // Component Type Info

            if (c_type.hash() == entt::type_hash<TagComponent>())
                continue;
            auto it = ComponentInspectors.find(c_type.hash());
            if (it == ComponentInspectors.end()) {
                ImGui::Text("Not Support for Component '%s' Inspect", c_type.name().data());
                continue;
            }
            
            std::string_view name = c_type.name();
            if (name.starts_with("struct ")) {
                name = name.substr(7);
            }
            if (name.ends_with("Component")) {
                name = name.substr(0, name.size() - 9);
            }
            static char _ComponentName[128];
            std::strncpy(_ComponentName, name.data(), name.size());
            _ComponentName[name.size()] = 0;

            ImGui::Spacing();
            bool open = ImGui::CollapsingHeader(_ComponentName);

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Remove Component"))
                {
                    c_storage.erase(entity);
                    open = false;  // already erased. skip access
                }
                ImGui::EndPopup();
            }

            if (open)
            {
                void* c_data = c_storage.value(entity);  // get the Component Data
                    
                it->second(c_data);
            }
    
        }
    }

    //if (ImGui::Button("Add Component", { -1, 0 })) {}


    ImGui::EndChild();

    ImGui::TextDisabled(std::format("{} components, entity id: {}.", numEntityComponents, entity.id()).c_str());


    ImGui::End();
}


#pragma endregion









#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

static void _InspTransform(TransformComponent& comp)
{
    glm::mat4 mat = comp.Transform;

    glm::vec3 scale;
    glm::quat quat;
    glm::vec3 pos;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(mat, scale, quat, pos, skew, perspective);

    //        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    //        ImGuizmo::DecomposeMatrixToComponents(&RenderEngine::matView[0][0], matrixTranslation, matrixRotation, matrixScale);
    //        ImGui::InputFloat3("Tr", matrixTranslation);
    //        ImGui::InputFloat3("Rt", matrixRotation);
    //        ImGui::InputFloat3("Sc", matrixScale);
    //        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &RenderEngine::matView[0][0]);


    ImGui::DragFloat3("Position", &comp.position().x);
    ImGui::DragFloat3("Rotation", &comp.position().x);
    ImGui::DragFloat3("Scale", &comp.position().x);



    ImGui::Separator();
    //ImGui::SeparatorText("Gizmo:");
    
    static ImGuizmo::OPERATION gizmoOp   = ImGuizmo::ROTATE;
    static ImGuizmo::MODE      gizmoMode = ImGuizmo::WORLD;
    if (ImGui::IsKeyPressed(ImGuiKey_W)) gizmoOp = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_E)) gizmoOp = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R)) gizmoOp = ImGuizmo::SCALE;
    
    if (ImGui::RadioButton("Translate", gizmoOp == ImGuizmo::TRANSLATE))    gizmoOp = ImGuizmo::TRANSLATE;  ImGui::SameLine();
    if (ImGui::RadioButton("Rotate",    gizmoOp == ImGuizmo::ROTATE))       gizmoOp = ImGuizmo::ROTATE;     ImGui::SameLine();
    if (ImGui::RadioButton("Scale",     gizmoOp == ImGuizmo::SCALE))        gizmoOp = ImGuizmo::SCALE;
    
    if (gizmoOp != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", gizmoMode == ImGuizmo::LOCAL)) gizmoMode = ImGuizmo::LOCAL; ImGui::SameLine();
        if (ImGui::RadioButton("World", gizmoMode == ImGuizmo::WORLD)) gizmoMode = ImGuizmo::WORLD;
    }
    
    static bool _OpSnap = false;
    bool snap = _OpSnap || ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftSuper);
    if (ImGui::Checkbox("Snap", &snap)) {
        _OpSnap = snap;
    }

    static glm::vec3 _SnapValue{ 0.5 };
    if (snap) {
        ImGui::DragFloat3("Snap value", &_SnapValue[0], 0.5f);
    }
    
    static bool _OpBound = false;
    ImGui::Checkbox("Bound", &_OpBound);
    
    static glm::vec3 _BoundSnapValue{0.5f};
    if (_OpBound) {
        ImGui::DragFloat3("Bound Snap value", &_BoundSnapValue[0], 0.5f);
    }
        
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    
    //bool manipulated = ImGuizmo::Manipulate(
    //    glm::value_ptr(Ethertia::getCamera().matView),
    //    glm::value_ptr(Ethertia::getCamera().matProjection),
    //    gizmoOp, gizmoMode,
    //    glm::value_ptr(mat),
    //    nullptr,  // delta mat?
    //    nullptr,  // snap
    //    _OpBound ? bounds : nullptr,  // bound
    //    _OpBound ? boundsSnap : nullptr);  // boundSnap
    //
    //if (manipulated) // ImGuizmo::IsUsing() 
    //{
    //    //Mth::decomposeTransform(matModel, pos, rot, scl);
    //
    //    comp.Transform = mat;
    //}
}




static void _InspDbgAABB(DebugDrawBoundingBox& comp)
{
    ImGui::DragFloat3("Min", &comp.BoundingBox.min.x);
    ImGui::DragFloat3("Max", &comp.BoundingBox.max.x);
    ImGui::ColorEdit4("Color", &comp.Color.x);
}


void ImwInspector::InitComponentInspectors()
{
    
    ImwInspector::AddComponentInspector<TransformComponent>(_InspTransform);

    ImwInspector::AddComponentInspector<DebugDrawBoundingBox>(_InspDbgAABB);
}

static void _InspWorld(World* world)
{
    WorldInfo& worldinfo = world->GetWorldInfo();

    ImGui::Text(std::format(
        "World Name: {}\n"
        "World Seed: {}\n"
        "DayTime: {}\n"
        "DayTime Length: {}\n"
        "Inhibited Time: {}\n"
        ,
        worldinfo.Name,
        worldinfo.Seed,
        worldinfo.InhabitedTime,
        worldinfo.InhabitedTime,
        worldinfo.InhabitedTime
    ).c_str());
    //ImGui::BeginDisabled();
    //const WorldInfo& winf = world->m_WorldInfo;
    //
    //static char WorldName[128];
    //ImGui::InputText("World Name", WorldName, 128);
    //
    //static char WorldSeed[128];
    //ImGui::InputText("World Seed", WorldSeed, 128);
    //
    //ImGui::EndDisabled();
    //
    //ImGui::SliderFloat("Day Time", &world->m_WorldInfo.DayTime, 0, 1);
    //ImGui::SliderFloat("Day Time Length", &world->m_WorldInfo.DayLength, 1, 3600);
    //
    //
    //ImGui::ColorEdit3("Sun Color", &Dbg::dbg_WorldSunColor.x);
    //ImGui::ColorEdit3("Dbg Color", &Dbg::dbg_ShaderDbgColor.x);
    //ImGui::DragFloat("Sun Brightness Mul", &Dbg::dbg_WorldSunColorBrightnessMul, 0.1);
    //
    //
    //if (ImGui::Button("Open World Save Directory"))
    //{
    //    Loader::OpenURL(world->m_ChunkLoader->m_ChunkDir);
    //}
    //if (ImGui::IsItemHovered()) {
    //    ImGui::SetTooltip("%s", world->m_ChunkLoader->m_ChunkDir.c_str());
    //}
}
