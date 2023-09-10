//
// Created by Dreamtowards on 2023/5/14.
//

#include "Imw.h"

#include <imgui_internal.h>
#include "ImKeymap.cpp"

#include <ethertia/init/KeyBinding.h>
#include <ethertia/world/ChunkLoader.h>
#include <ethertia/Ethertia.h>
#include <ethertia/util/Loader.h>
#include <ethertia/init/Settings.h>
#include <ethertia/init/MaterialTextures.h>


static ImGuiKey GetPressedKey()
{
    for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; ++k)
    {
        if (ImGui::IsKeyPressed((ImGuiKey)k))
            return (ImGuiKey)k;
    }
    return ImGuiKey_None;
}


void Imw::Settings::ShowSettings(bool* pOpen)
{
    ImGui::Begin("Settings", pOpen);

    Imw::Settings::Panel& currpanel = Imw::Settings::CurrentPanel;

    ImGui::BeginChild("SettingNav", {150, 0}, true);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {8, 8});
    {
#define ET_IMW_SETTINGS_LABEL(name, val) if (ImGui::RadioButton(name, currpanel==val)) { currpanel=val; }

        ET_IMW_SETTINGS_LABEL("Profile", Imw::Settings::Profile);
        ET_IMW_SETTINGS_LABEL("Current World", Imw::Settings::CurrentWorld);
        ImGui::Separator();
        ET_IMW_SETTINGS_LABEL("Graphics", Imw::Settings::Graphics);
        ET_IMW_SETTINGS_LABEL("Music & Sounds", Imw::Settings::Audio);
        ET_IMW_SETTINGS_LABEL("Controls", Imw::Settings::Controls);
        ET_IMW_SETTINGS_LABEL("Language", Imw::Settings::Language);
        ImGui::Separator();
        ET_IMW_SETTINGS_LABEL("Mods", Imw::Settings::Mods);
        ET_IMW_SETTINGS_LABEL("Assets", Imw::Settings::ResourcePacks);
        ImGui::Separator();
        ET_IMW_SETTINGS_LABEL("Credits", Imw::Settings::Credits);
        ET_IMW_SETTINGS_LABEL("Misc", Imw::Settings::Misc);
    }

    ImGui::PopStyleVar();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("SettingPanel", {0,0}, true);
    {
        if (currpanel==Profile)
        {
            ImGui::Dummy({0, 14});
            ImGui::SetWindowFontScale(1.5f);
            ImGui::Text("Dreamtowards");
            ImGui::SetWindowFontScale(0.9f);
            ImGui::TextDisabled("ref.dreamtowards@gmail.com");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Dummy({0, 8});
            ImGui::Button("Profile*");

            ImGui::Dummy({0, 64});

            ImGui::SeparatorText("Customize Character");
        }
        else if (currpanel==CurrentWorld)
        {
            World* world = Ethertia::getWorld();
            if (!world)
            {
                ImGui::TextDisabled("No world loaded.");
            }
            else
            {
                ImGui::BeginDisabled();
                const WorldInfo& winf = world->m_WorldInfo;

                static char WorldName[128];
                ImGui::InputText("World Name", WorldName, 128);

                static char WorldSeed[128];
                ImGui::InputText("World Seed", WorldSeed, 128);

                ImGui::EndDisabled();

                ImGui::SliderFloat("Day Time", &world->m_WorldInfo.DayTime, 0, 1);
                ImGui::SliderFloat("Day Time Length", &world->m_WorldInfo.DayLength, 1, 3600);


                ImGui::ColorEdit3("Sun Color", &Dbg::dbg_WorldSunColor.x);
                ImGui::ColorEdit3("Dbg Color", &Dbg::dbg_ShaderDbgColor.x);
                ImGui::DragFloat("Sun Brightness Mul", &Dbg::dbg_WorldSunColorBrightnessMul, 0.1);


                if (ImGui::Button("Open World Save Directory"))
                {
                    Loader::openURL(world->m_ChunkLoader->m_ChunkDir);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s", world->m_ChunkLoader->m_ChunkDir.c_str());
                }
            }

        }
        else if (currpanel==Graphics)
        {
//            ImGui::SeparatorText("Bloom");
//            ImGui::Checkbox("Bloom", &Settings::g_SSAO);



            ImGui::SeparatorText("General");

            int s_ProfileIdx = 0;

            ImGui::Combo("Profile", &s_ProfileIdx, "Custom", 1);

            ImGui::SliderFloat("FOV", &Ethertia::getCamera().fov, 0, 180);
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Field of View.\nNormal: 70\nQuark Pro: 90");

            int fpsLimit = ::Settings::s_Vsync ? 0 : ::Settings::s_FpsCap;
            ImGui::SliderInt("Vsync or FPS Limit", &fpsLimit, 0, 1000,
                             fpsLimit ? Strings::fmt("{} FPS", fpsLimit).c_str() : "Vsync");
            if (fpsLimit != 0) {
                ::Settings::s_FpsCap = fpsLimit;
                ::Settings::s_Vsync = false;
            } else {
                ::Settings::s_Vsync = true;
            }

            static float s_SlidingGuiScale = 0;
            float _gui_scale = Imgui::GuiScale;
            if (ImGui::SliderFloat("GUI Scale", &_gui_scale, 0.5f, 4.0f)) {
                s_SlidingGuiScale = _gui_scale;
            } else if (s_SlidingGuiScale != 0) {
                Imgui::GuiScale = s_SlidingGuiScale;
                s_SlidingGuiScale = 0;
            }

            ImGui::SliderFloat("Chunk Load Distance", &::Settings::s_ViewDistance, 0, 12);

#define SeparateRenderSection ImGui::Dummy({0, 6})



            SeparateRenderSection;
            ImGui::SeparatorText("Materials");


            if (ImGui::CollapsingHeader("Terrain Material"))
            {
//                ImGui::DragFloat("Texture Scale", &RendererGbuffer::g_uboFrag.MtlTexScale, 0.1);
//
//                ImGui::DragFloat("Texture Triplanar Blend Pow", &RendererGbuffer::g_uboFrag.MtlTriplanarBlendPow, 0.1);
//                if (ImGui::IsItemHovered()) ImGui::SetTooltip("Higher = 'Sharper' Normal Vectors");
//
//                ImGui::DragFloat("Texture Heightmap Blend Pow", &RendererGbuffer::g_uboFrag.MtlHeightmapBlendPow, 0.1);
//                if (ImGui::IsItemHovered()) ImGui::SetTooltip("Lower = More Intertexture at Material Blend");

                ImGui::DragInt("Texture Resolution", &MaterialTextures::TEX_RESOLUTION, 16, 2048);
                if (ImGui::IsItemHovered()) ImGui::SetTooltip("[Reload Required] for bake material texture atlas."
                                                              "\n64x  = Great Performance"
                                                              "\n128x = Optimal Performance"
                                                              "\n256x = Low-End Mobile Devices"
                                                              "\n512x = Normal"
                                                              "\n1024x= High-End PC");
            }


            SeparateRenderSection;
            ImGui::SeparatorText("Sky");

            if (ImGui::CollapsingHeader("Atmosphere"))
            {
            }

            if (ImGui::CollapsingHeader("Volumetric Cloud"))
            {
            }

            if (ImGui::CollapsingHeader("Exponential Fog"))
            {
            }

            if (ImGui::CollapsingHeader("Aurora"))
            {
            }

            if (ImGui::CollapsingHeader("Light Shafts"))
            {
            }

            SeparateRenderSection;
            ImGui::SeparatorText("Liquid");

            if (ImGui::CollapsingHeader("SSR"))
            {
            }

//            SeparateRenderSection;
//            ImGui::SeparatorText("Lighting");

            SeparateRenderSection;
            ImGui::SeparatorText("Lighting");

            if (ImGui::CollapsingHeader("Shadow Mapping"))
            {
                ImGui::Checkbox("Shadow Mapping", &::Settings::g_ShadowMapping);

                static int g_ShadowResolution = 1024;
                ImGui::SliderInt("Shadow Depth Map Resolution", &g_ShadowResolution, 128, 2048);

                ImGui::SliderInt("Shadow Distance", &g_ShadowResolution, 128, 2048);
            }



            SeparateRenderSection;
            ImGui::SeparatorText("Post Processing");


            if (ImGui::CollapsingHeader("SSAO"))
            {
                ImGui::Checkbox("SSAO", &::Settings::g_SSAO);
            }

            if (ImGui::CollapsingHeader("Bloom"))
            {
            }

            if (ImGui::CollapsingHeader("Lens Flare"))
            {
            }
            if (ImGui::CollapsingHeader("Dirty Lens"))
            {
            }

            if (ImGui::CollapsingHeader("Depth of Field"))
            {
            }

            if (ImGui::CollapsingHeader("Vignette"))
            {
            }

            if (ImGui::CollapsingHeader("Chromatic Aberration"))
            {
            }

            if (ImGui::CollapsingHeader("Anti-aliasing"))
            {
            }

            if (ImGui::CollapsingHeader("Motion Blur"))
            {
            }



            SeparateRenderSection;
            ImGui::SeparatorText("Color");


            if (ImGui::CollapsingHeader("Tonemap"))
            {
            }



//            ImGui::SeparatorText("SSAO");
//            ImGui::Checkbox("SSAO", &Settings::g_SSAO);
//            SeparateRenderSection;

//            ImGui::SeparatorText("Shadow Mapping");
//            ImGui::Checkbox("Shadow Mapping", &Settings::g_ShadowMapping);
//            static int g_ShadowResolution = 1024;
//            ImGui::SliderInt("Shadow Depth Map Resolution", &g_ShadowResolution, 128, 2048);
//            SeparateRenderSection;
//
//            ImGui::SeparatorText("Bloom");
//            ImGui::Checkbox("Bloom", &Settings::g_SSAO);
//            SeparateRenderSection;

        }
        else if (currpanel==Audio)
        {
            static int g_MasterVolume = 0;
            ImGui::SliderInt("Master Volume", &g_MasterVolume, 0, 100, "%d%%");
            ImGui::Dummy({0, 8});
            ImGui::SliderInt("Music", &g_MasterVolume, 0, 100, "%d%%");
        }
        else if (currpanel==Controls)
        {
//            ImGui::SeparatorText("Mouse");
//
//            static float s_MouseSensitivity = 1.0f;
//            ImGui::SliderFloat("Mouse Sensitivity", &s_MouseSensitivity, 0, 2);
//
//
//            ImGui::SeparatorText("Keyboard");

            static KeyBinding* s_HoveredKey = nullptr;


            ImGui::BeginChild("KeyBindingList", {270, 0});
            for (auto& it : KeyBinding::REGISTRY)
            {
                auto& name = it.first;
                KeyBinding* keyBinding = it.second;

                ImVec2 line_min = ImGui::GetCursorScreenPos();
                ImVec2 line_max = line_min + ImVec2{270, 18};
                if (ImGui::IsMouseHoveringRect(line_min, line_max) || s_HoveredKey == keyBinding) {
                    ImGui::RenderFrame(line_min, line_max, ImGui::GetColorU32(ImGuiCol_TitleBg));
                    s_HoveredKey = keyBinding;
                }

                ImGui::Text("%s", name.c_str());

                ImGui::SameLine(160);


                if (ImGui::Button(ImGui::GetKeyName(keyBinding->key()), {100, 0})) {}
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("press key to set");

                    ImGuiKey k = GetPressedKey();
                    if (k) {
                        keyBinding->m_Key = k;
                    }
                }

                if (keyBinding->key() != keyBinding->m_DefaultKey)
                {
                    ImGui::SameLine(132);

                    ImGui::PushID(name.c_str());
                    if (ImGui::Button("X"))
                    {
                        keyBinding->m_Key = keyBinding->m_DefaultKey;
                    }
                    ImGui::PopID();
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Reset (to: %s)", ImGui::GetKeyName(keyBinding->m_DefaultKey));
                    }
                }
            }
            ImGui::EndChild();

            ImGui::SameLine();


            bool updatedHoveredKey = false;
            ImKeymap::g_FuncKeyColor = [&updatedHoveredKey](ImGuiKey k, bool hover) -> ImU32 {
                for (auto& it : KeyBinding::REGISTRY) {
                    if (it.second->key() == k) {
                        if (hover) {
                            s_HoveredKey = it.second;
                            updatedHoveredKey = true;

                            ImGui::BeginTooltip();
                            ImGui::Text("%s", it.first.c_str());
                            ImGui::EndTooltip();
                        }
                        if (it.second == s_HoveredKey) {
                            return ImGui::GetColorU32(ImGuiCol_TitleBg);
                        }
                        return ImGui::GetColorU32(ImGuiCol_Header);
                    }
                }
                return -1;
            };
            ImKeymap::ShowKeymap();

            if (!updatedHoveredKey)
                s_HoveredKey = nullptr;
        }
        else if (currpanel==Language)
        {
            std::vector<std::pair<const char*, const char*>> g_Languages = {
                    {"en_us", "English"},
                    {"zh_tw", "Chinese (Complified)"},
                    {"zh_cn", "Chinese (Simplified)"}
            };
            static const char* g_SelectedLanguage = "en_us";

            ImGui::BeginChild("LangList", {0, 200});
            for (auto it : g_Languages) {
                if (ImGui::Selectable(it.second, g_SelectedLanguage==it.first)) {
                    g_SelectedLanguage = it.first;
                }
            }
            ImGui::EndChild();

            ImGui::TextDisabled("Translation may not 100%% accurate.");
        }
        else if (currpanel==Mods)
        {
            static bool s_ModValidatin = true;
            ImGui::Checkbox("Mod Files Hash Online Validation", &s_ModValidatin);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Only enable mods that passed official validation (need online check official database).\n"
                                  "It will check mod file (hash) whether passed official validation"
                                  "since native mods have big permission, at the same time of powerful, "
                                  "it also maybe damage your computer. so we check");
            // 检查Mod文件(hash摘要)是否通过官方验证 (需要联网检查官方数据库)
            // 由于Native-Mod的权限极大，在强大和高效能的同时，也可能会有恶意Mod损害您的电脑.
            // 开启Mod官方验证将会只启用通过官方验证的Mod
        }
        else if (currpanel==ResourcePacks)
        {
            if (ImGui::Button("Open ResourcePacks folder")) {
                Loader::openURL("./resourcepacks");
            }

            ImGui::BeginChild("AssetsList", {0, 200});
            for (auto& path : ::Settings::Assets) {
                ImGui::Selectable(path.c_str(), false);
            }
            ImGui::EndChild();

        }
        else if (currpanel==Credits)
        {
            ImGui::SetWindowFontScale(1.4f);
            ImGui::Text("%s", Ethertia::Version::name().c_str());
            ImGui::SetWindowFontScale(1.0f);

            ImGui::Text("Dev: Eldrine Le Prismarine");

            ImGui::SeparatorText("Links");
            ImGui::SmallButton("ethertia.com");
        }
    }
    ImGui::EndChild();

    ImGui::End();
}