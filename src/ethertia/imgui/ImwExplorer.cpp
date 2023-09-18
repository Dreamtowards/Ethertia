

#include "Imw.h"
#include <imgui_internal.h>

#include <stdx/stdx.h>
#include <set>

#include <filesystem>


static void _ShowFileDetailTooltip(const std::filesystem::path& path)
{
    ImGui::BeginTooltip();
    size_t filesize = std::filesystem::file_size(path);
    auto time = std::filesystem::last_write_time(path);
    ImGui::TextDisabled(
        "Path: %s\n"
        "Size: %s\n"
        "Date modified: %s",
        path.string().c_str(),
        stdx::size_str(filesize).c_str(),
        std::format("{}", time));
    ImGui::EndTooltip();
}

static void _ListFolder(const std::filesystem::path& _path, const std::string& displayname = {}, bool listFiles = true)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    std::string filename = displayname.empty() ? _path.filename().string() : displayname;

    if (std::filesystem::is_directory(_path))
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;// | ImGuiTreeNodeFlags_FramePadding;
        if (Imw::Editor::ExploringPath == _path)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }
        if (ImGui::TreeNodeEx(filename.c_str(), flags))
        {
            if (ImGui::IsItemClicked())
            {
                Imw::Editor::ExploringPath = _path;
            }

            // order: folders, then files
            std::vector<std::filesystem::path> ls;
            int i_folder_end = 0;
            for (auto& p : std::filesystem::directory_iterator(_path))
            {
                if (std::filesystem::is_directory(p))
                {
                    ls.insert(ls.begin() + i_folder_end, p);
                    ++i_folder_end;
                }
                else if (listFiles)
                {
                    ls.push_back(p);
                }
            }

            for (const std::filesystem::path& p : ls)
            {
                _ListFolder(p, {}, listFiles);
            }
            ImGui::TreePop();
        }
    }
    else
    {
        ImGui::TreeNodeEx(filename.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
    }

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
    {
        _ShowFileDetailTooltip(_path);
    }
}

static void _PathEdit()
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
    ImGui::BeginChild(ImGui::GetID("ImExpPath"), { 0, ImGui::GetTextLineHeightWithSpacing() });
    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor();
    static int  _EditPathMode = 0;  // 0: Buttons, 1= EditingText, 2=JustOpenEdit
    static char _EditPath[256];

    if (_EditPathMode)
    {
        if (_EditPathMode == 2)
        {
            ImGui::SetKeyboardFocusHere(0);  // focus Path InputText
            _EditPathMode = 1;
        }

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText("##ImwExpPathInput", _EditPath, std::size(_EditPath), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            Imw::Editor::ExploringPath = _EditPath;
        }
        ImGui::PopItemWidth();

        if (!ImGui::IsItemFocused())
        {
            _EditPathMode = 0;
        }
    }
    else
    {
        std::filesystem::path selpath;

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        std::filesystem::path abspath;
        for (const std::filesystem::path& part : Imw::Editor::ExploringPath)
        {
            abspath /= part;

            if (part == "\\")  // Windows: skip the "\" part after e.g. "C:"
                continue;

            if (ImGui::Button(part.string().c_str()))
            {
                selpath = abspath;
            }
            ImGui::SameLine(0, 0);

            ImGui::TextDisabled("/");
            //if (ImGui::ArrowButton(part.string().c_str(), ImGuiDir_Right)) { }
            ImGui::SameLine(0, 0);
        }
        ImGui::PopStyleColor();

        if (!selpath.empty())
        {
            Imw::Editor::ExploringPath = selpath;
        }
    }
    ImGui::EndChild();

    if (ImGui::IsItemClicked())
    {
        _EditPathMode = 2;
        std::strcpy(_EditPath, Imw::Editor::ExploringPath.string().c_str());
    }
}

void Imw::Editor::ShowExplorer(bool* _open)
{
    ImGui::Begin("Explorer", _open);

    ImGui::BeginChild("ImwExplorerOutline", { 300, 0 }, true);
    {
        static bool s_ListFiles = true;
        ImGui::Checkbox("ListFiles", &s_ListFiles);
        ImGui::SameLine();
        if (ImGui::SmallButton("+"))
        {

        }

        if (ImGui::BeginTable("ImwExplorerThumbnails", 1,
            ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoHostExtendX,
            { 300, 0 }))
        {
            //const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
            //ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
            //ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
            //ImGui::TableHeadersRow();

            _ListFolder(
                std::filesystem::current_path(),
                std::format("Current Path ({})", std::filesystem::current_path().string()),
                s_ListFiles);

            ImGui::EndTable();
        }
    }
    ImGui::EndChild();

    //ImGui::Separator();

    ImGui::SameLine();

    ImGui::BeginChild("ImwExplorerDetail", {}, true);
    {
        _PathEdit();

        static float s_ThumbnailsSize = 64;
        ImGui::BeginChild("ImwExplorerThumbnails", { 0, -ImGui::GetFrameHeightWithSpacing() });  // sizeY: remains bottom adjust bar

        if (std::filesystem::is_directory(Imw::Editor::ExploringPath))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 4 });
            for (const std::filesystem::path& p : std::filesystem::directory_iterator(Imw::Editor::ExploringPath))
            {
                ImGui::BeginGroup();// Child(p.filename().string().c_str(), { s_ThumbnailsSize , s_ThumbnailsSize + 64 });
                if (ImGui::Button("_", { s_ThumbnailsSize, s_ThumbnailsSize }))
                {

                }
                ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + s_ThumbnailsSize);
                ImGui::Text(p.filename().string().c_str());
                ImGui::PopTextWrapPos();

                ImGui::EndGroup();

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
                {
                    _ShowFileDetailTooltip(p);
                }

                if (ImGui::GetContentRegionMaxAbs().x - ImGui::GetItemRectMax().x > s_ThumbnailsSize + 10)
                {
                    ImGui::SameLine();
                }
            }
            ImGui::PopStyleVar(1);
        }

        ImGui::EndChild();

        ImGui::PushItemWidth(128);
        //ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin() + ImVec2{0, ImGui::GetWindowContentRegionMax().y - 18});
        ImGui::SliderFloat("Size", &s_ThumbnailsSize, 32, 320);
        ImGui::PopItemWidth();
    }
    ImGui::EndChild();

    ImGui::End();
}