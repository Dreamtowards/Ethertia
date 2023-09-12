//
// Created by Dreamtowards on 9/8/2023.
//

#include "Imw.h"

#include <imgui_internal.h>

#include <ethertia/util/Profiler.h>
#include <ethertia/Ethertia.h>
#include <ethertia/init/DebugStat.h>

static float _RenderProfilerSection(const Profiler::Section& sec, float x, float y, float full_width, float full_width_time, const std::function<float(const Profiler::Section&)>& timefunc)
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
        dx += _RenderProfilerSection(sub_sec, x+dx, y-LINE_HEIGHT, s_width, s_time, timefunc);
    }

    if (ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(min, max))
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


void Imw::Editor::ShowProfiler(bool* _open)
{
    ImGui::Begin("Profiler", _open);

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
    _RenderProfilerSection(sec, begin.x, begin.y, width, timefunc(sec), timefunc);

    ImGui::End();
}


