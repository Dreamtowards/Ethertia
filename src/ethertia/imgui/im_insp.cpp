//
// Created by Dreamtowards on 2023/5/14.
//


#include <ethertia/entity/passive/EntityChicken.h>
#include <ethertia/entity/EntityFurnace.h>



static void ShowEntityInsp()
{
    ImGui::Begin("Entity", &Settings::w_EntityInsp);

    Entity* entity = Imgui::g_InspEntity;
    if (!entity) {
        ImGui::TextDisabled("No entity selected.");
        ImGui::End();
        return;
    }
//    ImGui::BeginChild();

    ImGui::TextDisabled("%i components", (int)entity->numComponents());

    if (ImGui::CollapsingHeader("Transform")) {

        ImGui::DragFloat3("Position", &entity->position()[0]);
        ImGui::DragFloat3("Rotation", &entity->position()[0]);
        ImGui::DragFloat3("Scale", &entity->position()[0]);

        ImGui::Separator();
        ImGui::TextDisabled("Gizmo:");

        static ImGuizmo::OPERATION gizmoOp   = ImGuizmo::ROTATE;
        static ImGuizmo::MODE      gizmoMode = ImGuizmo::WORLD;
        if (ImGui::IsKeyPressed(ImGuiKey_T)) gizmoOp = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) gizmoOp = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_S)) gizmoOp = ImGuizmo::SCALE;

        if (ImGui::RadioButton("Translate", gizmoOp == ImGuizmo::TRANSLATE)) gizmoOp = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", gizmoOp == ImGuizmo::ROTATE)) gizmoOp = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", gizmoOp == ImGuizmo::SCALE)) gizmoOp = ImGuizmo::SCALE;

//        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
//        ImGuizmo::DecomposeMatrixToComponents(&RenderEngine::matView[0][0], matrixTranslation, matrixRotation, matrixScale);
//        ImGui::InputFloat3("Tr", matrixTranslation);
//        ImGui::InputFloat3("Rt", matrixRotation);
//        ImGui::InputFloat3("Sc", matrixScale);
//        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &RenderEngine::matView[0][0]);

        if (gizmoOp != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", gizmoMode == ImGuizmo::LOCAL)) gizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", gizmoMode == ImGuizmo::WORLD)) gizmoMode = ImGuizmo::WORLD;
        }

        static bool _Snap = false;
        bool snap = _Snap || ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftSuper);
        if (ImGui::Checkbox("Snap", &snap)) {
            _Snap = snap;
        }

        glm::vec3 snapValue{0.5};
        ImGui::DragFloat3("Snap value", &snapValue[0], 0.5f);

        static bool _Bound = false;
        ImGui::Checkbox("Bound", &_Bound);

        glm::vec3 boundSnapValue{0.5f};
        ImGui::DragFloat3("Bound Snap value", &snapValue[0], 0.5f);


        {
//            EntityComponentTransform& ct = entity->getComponent<EntityComponentTransform>();

            glm::mat4 matModel = Mth::matModel(entity->position(),
                                               entity->getRotation(),
                                               {1, 1, 1});

            static float bounds[]     = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
            static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };

            ImGuizmo::Manipulate(glm::value_ptr(Ethertia::getCamera().matView),
                                 glm::value_ptr(Ethertia::getCamera().matProjection),
                                 gizmoOp, gizmoMode,
                                 glm::value_ptr(matModel),
                                 nullptr,
                                 snap ? &snapValue[0] : nullptr,
                                 _Bound ? bounds : nullptr, _Bound ? boundsSnap : nullptr);
            if (ImGuizmo::IsUsing()) {
                // Decompose Transformation

                glm::vec3 pos, scl;
                glm::mat3 rot;
                Mth::decomposeTransform(matModel, pos, rot, scl);

                entity->position() = pos;
//                *(glm::mat3*)entity->rot() = rot;
            }
        }
    }

//    if (ImGui::CollapsingHeader("Diffuse Map")) {
//        if (entity->m_DiffuseMap)
//        {
//            ImGui::Image((void*)(intptr_t)entity->m_DiffuseMap->texId, {64, 64});
//        }
//    }




    ImGui::End();
}



static void CreateEntityToWorld(Entity* e)
{
    e->position() = Ethertia::getCamera().position;;
    Ethertia::getWorld()->addEntity(e);
}

static void ShowEntities()
{
    ImGui::Begin("Entities", &Settings::w_EntityList);

    World* world = Ethertia::getWorld();
    if (!world) {
        ImGui::TextDisabled("World not loaded.");
        ImGui::End();
        return;
    }
    auto& entities = world->m_Entities;

    if (ImGui::Button(" + ")) {
        ImGui::OpenPopup("new_entity");
    }
    if (ImGui::BeginPopup("new_entity")) {
        bool disabledDueNoWorld = Ethertia::getWorld() == nullptr;
        if (disabledDueNoWorld) {
            ImGui::TextDisabled("Disabled due No World Loaded");
            ImGui::BeginDisabled();
        }

        if (ImGui::BeginMenu("Vehicle")) {
            if (ImGui::MenuItem("Helicopter"))
            {
                CreateEntityToWorld(new EntityHelicopter());
            }
//            if (ImGui::MenuItem("DrivingSeat"))
//            {
//                CreateEntityToWorld(new EntityDrivingSeat());
//            }
//            if (ImGui::MenuItem("Propeller"))
//            {
//                CreateEntityToWorld(new EntityPropeller());
//            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Light")) {

        }
        if (ImGui::MenuItem("Mesh"))
        {
            CreateEntityToWorld(new EntityMesh());
        }
        if (ImGui::MenuItem("Chicken"))
        {
            CreateEntityToWorld(new EntityChicken());
        }
        if (ImGui::MenuItem("Furnace"))
        {
            CreateEntityToWorld(new EntityFurnace());
        }
        ImGui::SeparatorText("Items");

        for (auto& it : Item::REGISTRY)
        {
            if (ImGui::MenuItem(it.first.c_str()))
            {
                ItemStack stack(it.second, 10);
                Ethertia::getPlayer()->m_Inventory.putItemStack(stack);
            }
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ItemImage(it.second);
                ImGui::EndTooltip();
            }
        }

        if (disabledDueNoWorld) {
            ImGui::EndDisabled();
        }
        ImGui::EndPopup();
    }

    ImGui::SameLine();

    static bool _ShowOnlyInFrustum = true;
    static bool _SortByDistance = false;
    static int _ListLimit = 100;
    static bool _KeepSelectHitEntity = false;
    static bool _IgnoreChunkProxyEntities = true;
    if (ImGui::Button("..."))
    {
        ImGui::OpenPopup("entities_ops");
    }
    if (ImGui::BeginPopup("entities_ops"))
    {
        ImGui::Checkbox("List only In-Frustum", &_ShowOnlyInFrustum);
        ImGui::Checkbox("Sort by Distance", &_SortByDistance);
        ImGui::Checkbox("Keep Select HitEntity", &_KeepSelectHitEntity);
        ImGui::Checkbox("Ignore chunk proxy entities", &_IgnoreChunkProxyEntities);
        ImGui::SliderInt("List Limit", &_ListLimit, 0, 5000);


        ImGui::TextDisabled("%i rendered / %i loaded.", Dbg::dbg_NumEntityRendered, (int)entities.size());

        if (ImGui::Button("Unselect")) {
            Imgui::g_InspEntity = nullptr;
        }

        ImGui::EndPopup();
    }

    if (_KeepSelectHitEntity) {
        auto& cur = Ethertia::getHitCursor();
        if (cur.hitEntity) {
            Imgui::g_InspEntity = cur.hitEntity;
        }
    }

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

    {
        int i = 0;
        for (Entity* e : entities)
        {
            if (_ListLimit != 0 && i > _ListLimit)
                break;
            if (_ShowOnlyInFrustum && !Ethertia::getCamera().testFrustum(e->getAABB()))
                continue;
            if (_IgnoreChunkProxyEntities && dynamic_cast<EntityMesh*>(e))
                continue;

            char buf[32];

            sprintf(buf, "#%.3i | %s", i, typeid(*e).name());
            if (ImGui::Selectable(buf, Imgui::g_InspEntity == e)) {
                Imgui::g_InspEntity = e;
            }
            ++i;
        }
    }


    ImGui::End();
}


void ShowNodeEditor()
{
    ImGui::Begin("NodeEdit", &w_NodeEditor);

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










static float RenderProfilerSection(const Profiler::Section& sec, float x, float y, float full_width, float full_width_time, const std::function<float(const Profiler::Section&)>& timefunc)
{
    const float LINE_HEIGHT = 16;

    double s_time = timefunc(sec);
    double s_width = (s_time / full_width_time) * full_width;
    glm::vec4 color = Colors::ofRGB(std::hash<std::string>()(sec.name) * 256);
    ImU32 col = ImGui::GetColorU32({color.x, color.y, color.z, color.w});

    ImVec2 min = {x,y};
    ImVec2 max = min+ImVec2(s_width, LINE_HEIGHT);
    ImGui::RenderFrame(min, max, col);
    ImGui::RenderText(min, sec.name.c_str());

    if (s_width > 180) {
        std::string str = Strings::fmt("{}ms@{}", s_time * 1000.0, sec.numExec);
        ImVec2 text_size = ImGui::CalcTextSize(str.c_str());
        ImGui::RenderText({max.x-text_size.x, min.y}, str.c_str());
    }

    float dx = 0;
    for (const Profiler::Section& sub_sec : sec.sections)
    {
        dx += RenderProfilerSection(sub_sec, x+dx, y-LINE_HEIGHT, s_width, s_time, timefunc);
    }

    if (ImGui::IsWindowFocused() && ImGui::IsMouseHoveringRect(min, max))
    {
        ImGui::SetTooltip("%s\n"
                          "\n"
                          "avg %fms\n"
                          "las %fms\n"
                          "sum %fms\n"
                          "exc %u\n"
                          "%%p  %f",
                          sec.name.c_str(),
                          sec._avgTime * 1000.0f,
                          sec._lasTime * 1000.0f,
                          sec.sumTime  * 1000.0f,
                          (uint32_t)sec.numExec,
                          (float)(sec.parent ? sec.sumTime / sec.parent->sumTime : Mth::NaN));
    }

    return s_width;
}

static void ShowProfilers()
{
    ImGui::Begin("Profiler");

    static int s_SelectedProfilerIdx = 0;
    static std::pair<const char*, Profiler*> PROFILERS[] = {
            {"MainLoop", &Ethertia::getProfiler()},
            {"Chunk Mesh", &Dbg::dbgProf_ChunkMeshGen},
            {"Chunk Gen/Load/Save", &Dbg::dbgProf_ChunkGen}
    };


    ImGui::SetNextItemWidth(200);
    if (ImGui::BeginCombo("###Profiler", PROFILERS[s_SelectedProfilerIdx].first))
    {
        for (int i = 0; i < std::size(PROFILERS); ++i)
        {
            if (ImGui::Selectable(PROFILERS[i].first, s_SelectedProfilerIdx == i)) {
                s_SelectedProfilerIdx = i;
            }
        }
        ImGui::EndCombo();
    }


    ImGui::SameLine();

    static int s_SelectedTimeFunc = 0;
    static std::pair<const char*, std::function<float(const Profiler::Section& sec)>> s_TimeFuncs[] = {
            {"AvgTime", [](const Profiler::Section& sec) { return sec._avgTime; }},
            {"SumTime", [](const Profiler::Section& sec) { return sec.sumTime; }},
            {"LastTime",[](const Profiler::Section& sec) { return sec._lasTime; }}
    };
    ImGui::SetNextItemWidth(120);
    if (ImGui::BeginCombo("###ProfilerTimeFunc", s_TimeFuncs[s_SelectedTimeFunc].first))
    {
        for (int i = 0; i < std::size(s_TimeFuncs); ++i)
        {
            if (ImGui::Selectable(s_TimeFuncs[i].first, s_SelectedTimeFunc == i)) {
                s_SelectedTimeFunc = i;
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();

    Profiler& prof = *PROFILERS[s_SelectedProfilerIdx].second;

    if (ImGui::Button("Reset"))
    {
        prof.laterClearRootSection();
    }

    const Profiler::Section& sec = prof.GetRootSection();
    auto& timefunc = s_TimeFuncs[s_SelectedTimeFunc].second;
    ImVec2 begin = ImGui::GetWindowPos() + ImVec2{ImGui::GetWindowContentRegionMin().x, ImGui::GetWindowContentRegionMax().y - 16};
    float width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
    RenderProfilerSection(sec, begin.x, begin.y, width, timefunc(sec), timefunc);

    ImGui::End();
}





static void ShowShaderProgramInsp()
{
    ImGui::Begin("ShaderProgram", &Settings::w_Settings);
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

    ImGui::End();
}
