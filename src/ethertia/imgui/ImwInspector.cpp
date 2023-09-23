//
// Created by Dreamtowards on 2023/5/14.
//

#include "Imw.h"

#include <ethertia/Ethertia.h>
#include <ethertia/init/Settings.h>
#include <ethertia/init/DebugStat.h>

//#include <ethertia/entity/Entity.h>
//#include <ethertia/entity/EntityMesh.h>
//#include <ethertia/entity/player/EntityPlayer.h>
//#include <ethertia/entity/passive/EntityChicken.h>
//#include <ethertia/entity/EntityFurnace.h>


#pragma region Imw Inspector


//static void _InspEntity()
//{
//
//    ImGui::TextDisabled("%i components", (int)entity->numComponents());
//
//    if (ImGui::CollapsingHeader("Transform")) {
//
//        ImGui::DragFloat3("Position", &entity->position()[0]);
//        ImGui::DragFloat3("Rotation", &entity->position()[0]);
//        ImGui::DragFloat3("Scale", &entity->position()[0]);
//
//        ImGui::Separator();
//        ImGui::TextDisabled("Gizmo:");
//
//        static ImGuizmo::OPERATION gizmoOp   = ImGuizmo::ROTATE;
//        static ImGuizmo::MODE      gizmoMode = ImGuizmo::WORLD;
//        if (ImGui::IsKeyPressed(ImGuiKey_T)) gizmoOp = ImGuizmo::TRANSLATE;
//        if (ImGui::IsKeyPressed(ImGuiKey_R)) gizmoOp = ImGuizmo::ROTATE;
//        if (ImGui::IsKeyPressed(ImGuiKey_S)) gizmoOp = ImGuizmo::SCALE;
//
//        if (ImGui::RadioButton("Translate", gizmoOp == ImGuizmo::TRANSLATE)) gizmoOp = ImGuizmo::TRANSLATE;
//        ImGui::SameLine();
//        if (ImGui::RadioButton("Rotate", gizmoOp == ImGuizmo::ROTATE)) gizmoOp = ImGuizmo::ROTATE;
//        ImGui::SameLine();
//        if (ImGui::RadioButton("Scale", gizmoOp == ImGuizmo::SCALE)) gizmoOp = ImGuizmo::SCALE;
//
////        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
////        ImGuizmo::DecomposeMatrixToComponents(&RenderEngine::matView[0][0], matrixTranslation, matrixRotation, matrixScale);
////        ImGui::InputFloat3("Tr", matrixTranslation);
////        ImGui::InputFloat3("Rt", matrixRotation);
////        ImGui::InputFloat3("Sc", matrixScale);
////        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &RenderEngine::matView[0][0]);
//
//        if (gizmoOp != ImGuizmo::SCALE)
//        {
//            if (ImGui::RadioButton("Local", gizmoMode == ImGuizmo::LOCAL)) gizmoMode = ImGuizmo::LOCAL;
//            ImGui::SameLine();
//            if (ImGui::RadioButton("World", gizmoMode == ImGuizmo::WORLD)) gizmoMode = ImGuizmo::WORLD;
//        }
//
//        static bool _Snap = false;
//        bool snap = _Snap || ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftSuper);
//        if (ImGui::Checkbox("Snap", &snap)) {
//            _Snap = snap;
//        }
//
//        glm::vec3 snapValue{0.5};
//        ImGui::DragFloat3("Snap value", &snapValue[0], 0.5f);
//
//        static bool _Bound = false;
//        ImGui::Checkbox("Bound", &_Bound);
//
//        glm::vec3 boundSnapValue{0.5f};
//        ImGui::DragFloat3("Bound Snap value", &snapValue[0], 0.5f);
//
//
//        {
////            EntityComponentTransform& ct = entity->getComponent<EntityComponentTransform>();
//
//            glm::mat4 matModel = Mth::matModel(entity->position(),
//                                               entity->getRotation(),
//                                               {1, 1, 1});
//
//            static float bounds[]     = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
//            static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
//
//            ImGuizmo::Manipulate(glm::value_ptr(Ethertia::getCamera().matView),
//                                 glm::value_ptr(Ethertia::getCamera().matProjection),
//                                 gizmoOp, gizmoMode,
//                                 glm::value_ptr(matModel),
//                                 nullptr,
//                                 snap ? &snapValue[0] : nullptr,
//                                 _Bound ? bounds : nullptr, _Bound ? boundsSnap : nullptr);
//            if (ImGuizmo::IsUsing()) {
//                // Decompose Transformation
//
//                glm::vec3 pos, scl;
//                glm::mat3 rot;
//                Mth::decomposeTransform(matModel, pos, rot, scl);
//
//                entity->position() = pos;
////                *(glm::mat3*)entity->rot() = rot;
//            }
//        }
//    }
//
////    if (ImGui::CollapsingHeader("Diffuse Map")) {
////        if (entity->m_DiffuseMap)
////        {
////            ImGui::Image((void*)(intptr_t)entity->m_DiffuseMap->texId, {64, 64});
////        }
////    }
//
//
//}

#include <format>
#include <glm/gtx/string_cast.hpp>

void Imw::Editor::ShowInspector(bool* _open)
{
    ImGui::Begin("Inspector", _open);

    void* inspectionObject = Imw::Editor::_InspectionObject;
    auto inspectionType = Imw::Editor::_InspectionType;
    if (!inspectionObject) {
        ImGui::TextDisabled("Nothing inspecting.");
        ImGui::End();
        return;
    }
    else
    {
        ImGui::TextDisabled(std::format("inspecting: {}.", inspectionObject).c_str());
        ImGui::SameLine();
        if (ImGui::SmallButton("x")) {
            Imw::Editor::SetInspectionObject(nullptr, eNone);
        }
        ImGui::Separator();
    }

    switch (inspectionType)
    {
        case eEntity: 
        {
            Entity* entity = (Entity*)inspectionObject;

            //ImGui::Text(std::format("Entity: {}", glm::to_string(entity->position())).c_str());
            break;
        }
        case eCamera:
        {
            Camera* cam = (Camera*)inspectionObject;

            ImGui::DragFloat3("Position", &cam->position.x);
            ImGui::DragFloat3("Euler Angles", &cam->eulerAngles.x);

            ImGui::SeparatorText("Perspective");
            ImGui::DragFloat("Fov", &cam->fov);
            ImGui::DragFloat("NearPlane", &cam->fov);
            ImGui::DragFloat("FarPlane", &cam->fov);
            break;
        }
        case ePipeline:
        {

            //break;
        }
        default:
        {
            ImGui::TextDisabled("Unsupported Inspecting type. %s", inspectionObject);
            ImGui::End();
            return;
        }
    }

    ImGui::End();
}





#pragma endregion






#pragma region Imw Hierarchy


//static void _CreateEntityToWorld(Entity* e)
//{
//    e->position() = Ethertia::getCamera().position;;
//    Ethertia::getWorld()->addEntity(e);
//}

static void _HrcEntities()
{
    World* world = Ethertia::GetWorld();
    if (!world) {
        ImGui::TextDisabled("World not loaded.");
        return;
    }
    //auto& entities = world->m_Entities;

    if (ImGui::Button(" + ")) {
        ImGui::OpenPopup("ImwEditorNewEntity");
    }
    if (ImGui::BeginPopup("ImwEditorNewEntity")) {
        //        if (ImGui::BeginMenu("Vehicle")) {
        ////            if (ImGui::MenuItem("Helicopter"))
        ////            {
        ////                CreateEntityToWorld(new EntityHelicopter());
        ////            }
        ////            if (ImGui::MenuItem("DrivingSeat"))
        ////            {
        ////                CreateEntityToWorld(new EntityDrivingSeat());
        ////            }
        ////            if (ImGui::MenuItem("Propeller"))
        ////            {
        ////                CreateEntityToWorld(new EntityPropeller());
        ////            }
        //            ImGui::EndMenu();
        //        }
        //        if (ImGui::MenuItem("Light")) {
        //
        //        }
        //        if (ImGui::MenuItem("Mesh"))
        //        {
        //            CreateEntityToWorld(new EntityMesh());
        //        }
        //        if (ImGui::MenuItem("Chicken"))
        //        {
        //            CreateEntityToWorld(new EntityChicken());
        //        }
        //        if (ImGui::MenuItem("Furnace"))
        //        {
        //            CreateEntityToWorld(new EntityFurnace());
        //        }
        
        //ImGui::SeparatorText("Items");
        //
        //for (auto& it : Item::REGISTRY)
        //{
        //    if (ImGui::MenuItem(it.first.c_str()))
        //    {
        //        ItemStack stack(it.second, 10);
        //        Ethertia::getPlayer()->m_Inventory.putItemStack(stack);
        //    }
        //    if (ImGui::IsItemHovered()) {
        //        ImGui::BeginTooltip();
        //        Imw::ItemImage(it.second);
        //        ImGui::EndTooltip();
        //    }
        //}

        ImGui::EndPopup();
    }

    //ImGui::SameLine();
    //
    //static bool _ShowOnlyInFrustum = true;
    //static bool _SortByDistance = false;
    //static int _ListLimit = 100;
    //static bool _KeepSelectHitEntity = false;
    //static bool _IgnoreChunkProxyEntities = true;
    //if (ImGui::Button("..."))
    //{
    //    ImGui::OpenPopup("entities_ops");
    //}
    //if (ImGui::BeginPopup("entities_ops"))
    //{
    //    ImGui::Checkbox("List only In-Frustum", &_ShowOnlyInFrustum);
    //    ImGui::Checkbox("Sort by Distance", &_SortByDistance);
    //    ImGui::Checkbox("Keep Select HitEntity", &_KeepSelectHitEntity);
    //    ImGui::Checkbox("Ignore chunk proxy entities", &_IgnoreChunkProxyEntities);
    //    ImGui::SliderInt("List Limit", &_ListLimit, 0, 5000);
    //
    //
    //    ImGui::TextDisabled("%i rendered / %i loaded.", Dbg::dbg_NumEntityRendered, (int)entities.size());
    //
    //    if (ImGui::Button("Unselect")) {
    //        Imgui::g_InspEntity = nullptr;
    //    }
    //
    //    ImGui::EndPopup();
    //}
    //
    //if (_KeepSelectHitEntity) {
    //    auto& cur = Ethertia::getHitCursor();
    //    if (cur.hitEntity) {
    //        Imgui::g_InspEntity = cur.hitEntity;
    //    }
    //}

    //    ImGui::SameLine();
    //    if (ImGui::TreeNode(".."))
    //    {
    //        ImGui::Checkbox("Show only In Frustum", &_ShowOnlyInFrustum);
    //        ImGui::Checkbox("Sort by Distance", &_SortByDistance);
    //        ImGui::SliderInt("List Limit", &_ListLimit, 0, 5000);
    //
    //        ImGui::TextDisabled("%i rendered / %i loaded.", Settings::dbgEntitiesRendered, (int)entities.size());
    //
    //        ImGui::TreePop();
    //    }


    ImGui::Separator();

    //{
    //    int i = 0;
    //    for (Entity* e : entities)
    //    {
    //        if (_ListLimit != 0 && i > _ListLimit)
    //            break;
    //        if (_ShowOnlyInFrustum && !Ethertia::getCamera().testFrustum(e->getAABB()))
    //            continue;
    //        if (_IgnoreChunkProxyEntities && dynamic_cast<EntityMesh*>(e))
    //            continue;
    //
    //        char buf[32];
    //
    //        sprintf(buf, "#%.3i | %s", i, typeid(*e).name());
    //        if (ImGui::Selectable(buf, Imgui::g_InspEntity == e)) {
    //            Imgui::g_InspEntity = e;
    //        }
    //        ++i;
    //    }
    //}
}

void Imw::Editor::ShowHierarchy(bool* _open)
{
    ImGui::Begin("Hierarchy", _open);

    static const char* types[] = { "Entities", "Pipelines", "Internals" };
    static int current = 0;

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::BeginCombo("##HierarchyType", types[current]))
    {
        for (size_t i = 0; i < std::size(types); i++)
        {
            if (ImGui::Selectable(types[i], i == current))
                current = i;
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();

    if (current == 0)
    {
        _HrcEntities();
    }
    else if (current == 1)
    {

    }
    else if (current == 2)
    {
        if (ImGui::Selectable("Main Camera")) {
            Imw::Editor::SetInspectionObject(&Ethertia::getCamera(), eCamera);
        }

        if (ImGui::Selectable("Pipeline: GBuffer")) {
            Imw::Editor::SetInspectionObject(&Ethertia::getCamera(), eCamera);
        }
    }


    ImGui::End();
}





#pragma endregion





#include <imgui-imnodes/imnodes.h>


void Imw::Editor::ShowWorldGen(bool* _open)
{
    ImGui::Begin("WorldGen", _open);

    // id of Attrib/Pin
    static std::vector<std::pair<int, int>> g_Links;

    ImNodes::BeginNodeEditor();

    for (int i = 0; i < 4; ++i) {
        ImNodes::BeginNode(i);

        ImNodes::BeginNodeTitleBar();
        ImGui::Text("Title");
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginInputAttribute(i*5+1);

        ImGui::Text("Input3");

        ImNodes::EndInputAttribute();

        ImNodes::BeginOutputAttribute(i*5+2, ImNodesPinShape_Triangle);
        static bool bl;
        ImGui::Checkbox("SthCheck", &bl);
        ImNodes::EndOutputAttribute();

        ImNodes::BeginStaticAttribute(i*5+3);
        ImGui::Text("Static");
        ImNodes::EndStaticAttribute();

        ImNodes::EndNode();
    }

    {
        int i = 0;
        for (auto& lk : g_Links) {
            ImNodes::Link(++i, lk.first, lk.second);
        }
    }
    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomLeft);  // call before ImNodes::EndNodeEditor();
    ImNodes::EndNodeEditor();

    {
        int attr_beg, attr_end;
        if (ImNodes::IsLinkCreated(&attr_beg, &attr_end)) {  // call after ImNodes::EndNodeEditor();
            g_Links.emplace_back(attr_beg, attr_end);
        }
    }


    ImGui::End();
}










//static void ShowShaderProgramInsp()
//{
//    ImGui::Begin("ShaderProgram", &Settings::w_Settings);
//
//
//    ShaderProgram* shader = g_InspShaderProgram;
//    if (ImGui::BeginCombo("###Shaders", shader ? shader->m_SourceLocation.c_str() : nullptr))
//    {
//        for (auto& it : ShaderProgram::REGISTRY)
//        {
//            if (ImGui::Selectable(it.first.c_str(), g_InspShaderProgram == it.second)) {
//                g_InspShaderProgram = it.second;
//            }
//        }
//        ImGui::EndCombo();
//    }
//    ImGui::SameLine();
//    ImGui::Button("+");
//
//    ImGui::Separator();
//
//    if (!shader) {
//        ImGui::TextDisabled("No ShaderProgram selected.");
//        ImGui::End();
//        return;
//    }
//
//    if (ImGui::Button("Reload Shader")) {
//        shader->reload_sources_by_filenames();
//        Log::info("Shader {} reloaded.", shader->m_SourceLocation);
//    }
//    ImGui::SameLine();
//    ImGui::Text("#%i",(int)shader->m_ProgramId);
//
//    ImGui::TextDisabled("%i Uniforms:", (int)shader->m_Uniforms.size());
//    for (auto& it : shader->m_Uniforms)
//    {
//        auto& unif = it.second;
//        const char* name = it.first;
//        if (!unif.value_ptr) {
//            ImGui::TextDisabled("uniform %s has no lvalue", name);
//            continue;
//        }
//        switch (unif.type)
//        {
//            case ShaderProgram::Uniform::INT:
//                ImGui::DragInt(name, (int*)unif.value_ptr, -100, 100);
//                break;
//            case ShaderProgram::Uniform::FLOAT:
//                ImGui::DragFloat(name, (float*)unif.value_ptr, -100, 100);
//                break;
//            case ShaderProgram::Uniform::VEC3:
//                ImGui::DragFloat3(name, (float*)unif.value_ptr, -100, 100);
//                break;
//            case ShaderProgram::Uniform::VEC4:
//                ImGui::DragFloat4(name, (float*)unif.value_ptr, -100, 100);
//                break;
//            case ShaderProgram::Uniform::MAT3:
//                ImGui::Text("Mat3");
//                break;
//            case ShaderProgram::Uniform::MAT4:
//                ImGui::Text("Mat4");
//                break;
//            default:
//                ImGui::Text("Unknown Uniform Type");
//                break;
//        }
//    }
//
//    ImGui::End();
//}
