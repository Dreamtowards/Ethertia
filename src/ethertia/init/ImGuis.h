//
// Created by Dreamtowards on 2023/3/3.
//

#ifndef ETHERTIA_IMGUIS_H
#define ETHERTIA_IMGUIS_H

class ImGuis
{
public:

    static void Init()
    {
        BENCHMARK_TIMER;
        Log::info("Init ImGui.. \1");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& imgui_io = ImGui::GetIO();

        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(Ethertia::getWindow()->m_WindowHandle, true);
        ImGui_ImplOpenGL3_Init("#version 150");  // GL 3.2 + GLSL 150


        ImFontConfig fontConf;
        fontConf.OversampleH = 2;
        fontConf.OversampleV = 2;
        fontConf.GlyphExtraSpacing.x = 1.0f;
//        fontConf.RasterizerMultiply = 2;
        imgui_io.Fonts->AddFontFromFileTTF("./assets/font/menlo.ttf", 14.0f, &fontConf);

//        imgui_io.DisplaySize = ImVec2(1920, 1080);
//        imgui_io.DeltaTime = 1.0f / 60.0f;

        ImGuiStyle& style = ImGui::GetStyle();
        style.GrabMinSize = 7;
        style.FrameBorderSize = 1;
        style.WindowMenuButtonPosition = ImGuiDir_Right;
        style.SeparatorTextBorderSize = 2;

        style.ScrollbarSize = 10;
        style.ScrollbarRounding = 2;
        style.TabRounding = 2;

        for (int i = 0; i < ImGuiCol_COUNT; ++i)
        {
            ImVec4& col = style.Colors[i];
            float f = Colors::luminance({col.x, col.y, col.z});
            //(col.x + col.y + col.z) / 3.0f;
            col = ImVec4(f,f,f,col.w);
        }


        style.Colors[ImGuiCol_CheckMark] =
        style.Colors[ImGuiCol_SliderGrab] =
                {1.000f, 1.000f, 1.000f, 1.000f};

        style.Colors[ImGuiCol_MenuBarBg] = {0,0,0,0};

        style.Colors[ImGuiCol_HeaderHovered] = {0.051f, 0.431f, 0.992f, 1.000f};
        style.Colors[ImGuiCol_HeaderActive] = {0.071f, 0.388f, 0.853f, 1.000f};

//        style.Colors[ImGuiCol_TitleBg] = {0.297f, 0.297f, 0.298f, 1.000f};
//        style.Colors[ImGuiCol_Button] =
//        style.Colors[ImGuiCol_Header] =
//        style.Colors[ImGuiCol_FrameBg] =
//                {0.322f, 0.322f, 0.322f, 0.540f};
//
//        style.Colors[ImGuiCol_ButtonHovered] =
//        style.Colors[ImGuiCol_HeaderHovered] =
//        style.Colors[ImGuiCol_FrameBgHovered] =
//                {0.626f, 0.626f, 0.626f, 0.400f};
//
//        style.Colors[ImGuiCol_ButtonActive] =
//        style.Colors[ImGuiCol_HeaderActive] =
//                {0.170f, 0.170f, 0.170f, 1.000f};
    }

    static void ShowMainMenuBar()
    {
        static bool g_ShowImGuiDemoWindow = false;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
//            ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5));
//            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Ethertia")) {
                if (ImGui::MenuItem("About...", nullptr)) {}
                if (ImGui::MenuItem("Shortcuts...", nullptr)) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Settings...", nullptr)) {}
                ImGui::Separator();
                if (ImGui::MenuItem("New World...", nullptr)) {}
                if (ImGui::MenuItem("Open World...", nullptr)) {}
                if (ImGui::MenuItem("Save World", nullptr)) {}
                if (ImGui::MenuItem("Exit World", nullptr)) {}

                ImGui::Separator();
                if (ImGui::MenuItem("Terminate", nullptr)) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Inspection")) {}
                if (ImGui::MenuItem("Loaded Entities")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Debug Panel")) {}
                if (ImGui::MenuItem("Profiler")) {}
                if (ImGui::MenuItem("ImGui Demo Window", nullptr, &g_ShowImGuiDemoWindow)) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("World"))
            {
                if (ImGui::MenuItem("Entities")) {}
                if (ImGui::MenuItem("Chunks")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
        ImGui::PopStyleVar();
//            ImGui::PopStyleVar();
        ImGui::PopStyleColor();
//            ImGui::PopStyleColor();



        if (g_ShowImGuiDemoWindow)
            ImGui::ShowDemoWindow(&g_ShowImGuiDemoWindow);

//            ImGui::Begin("My First Tool", &act, ImGuiWindowFlags_MenuBar);
//            ImGui::Text("Hello, world %d", 123);
//            if (ImGui::Button("Save"))
//                Log::info("Saved");
////            ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
//            static float f = 0;
//            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
//            ImGui::End();

    }

};

#endif //ETHERTIA_IMGUIS_H
