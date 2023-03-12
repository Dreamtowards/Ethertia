//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUISCREENMAINMENU_H
#define ETHERTIA_GUISCREENMAINMENU_H

#include "../GuiCommon.h"
#include "GuiScreenSingleplayer.h"
#include <ethertia/gui/settings/GuiScreenSettings.h>

#include <ethertia/render/Camera.h>
#include <ethertia/world/Biome.h>


class GuiScreenMainMenu : public GuiCollection
{
public:
    DECL_Inst(GuiScreenMainMenu);

    GuiScreenMainMenu() {
        setWidthHeight(Inf, Inf);

        GuiStack* vstack = new GuiStack(GuiStack::D_VERTICAL, 16);

        GuiButton* btnSingleplayer = new GuiButton("Singleplayer", false);
        vstack->addGui(btnSingleplayer);
        btnSingleplayer->addOnClickListener([](auto) {
            if (Ethertia::getWindow()->isAltKeyDown())
            {
                WorldInfo worldinfo;
                worldinfo.Name = "Internal Test";
                worldinfo.Seed = 1234;
                Ethertia::loadWorld("saves/world1", &worldinfo);
            }
            else
            {
                Ethertia::getRootGUI()->addGui(GuiScreenSingleplayer::Inst());
                GuiScreenSingleplayer::Inst()->refreshWorldList();
            }
        });

        GuiButton* btnMultiplayer = new GuiButton("Multiplayer", false);
        vstack->addGui(btnMultiplayer);
        btnMultiplayer->addOnClickListener([](auto) {
            const char* r = Loader::showInputBox("URL", "File, Folder, or URL", "");
            if (r) {
                Loader::openURL(r);
            }

        });

        GuiButton* btnSettings = new GuiButton("Settings", false);
        vstack->addGui(btnSettings);
        btnSettings->addOnClickListener([](auto) {

            Ethertia::getRootGUI()->addGui(GuiScreenSettings::Inst());
        });

        GuiButton* btnExit = new GuiButton("Terminate", false);
        vstack->addGui(btnExit);
        btnExit->addOnClickListener([](auto) {
            Ethertia::shutdown();
        });

        addGui(new GuiAlign(0.5, 0.5, vstack));  // x 0.12
    }

    void implDraw() override {

        Gui::drawRect({
            .xywh = xywh(),
            .color = Colors::WHITE80
        });

        Gui::drawString(0, Gui::maxHeight(), "0 mods loaded.\n" + Ethertia::Version::name(), Colors::WHITE60, 16,
                        {0.0f, -1.0f});


        Gui::drawString(Gui::maxWidth(), Gui::maxHeight(),
                        "Seasonally Excellent Dev: Eldrine Le Prismarine.\n"
                        "Copyright (c) Elytra Corporation. Do not distribute!",
                        Colors::WHITE60, 16, {-1.0, -1.0f});



//        drawAudioFreq();
//        drawBiomeDistribution();

    }


    // Biome Distribution of Temperature and Humidity
    void drawBiomeDistribution()
    {
        using glm::vec3;

        float x = 10, y = 100, w = 200, h = 200;
        float grain = 4;
        float grainW = w / grain, grainH = h / grain;

        Gui::drawRect(x, y, w, h, Colors::BLACK30);
        Gui::drawString(x, y + h + 8, "Biome Distribution of \nTemperature(x) and Humidity(y)");

        static Texture *TEX_HEAT = Loader::loadTexture("gui/map_heat.png");
        static Texture *TEX_HUMI = Loader::loadTexture("gui/map_humidity.png");

        float grad_tk = 3;  // thickness
        float grad_sp = 2;  // spacing
        Gui::drawRect(x, y + h + grad_sp, w, grad_tk, TEX_HEAT);  // Temp  X
        Gui::drawRect(x + w + grad_sp, y, grad_tk, h, TEX_HUMI);  // Humid Y

        std::unordered_map<std::string, std::pair<float, glm::vec2>> bioCenters;  // dist and rel-center-pos

        for (float rx = 0; rx < grainW; ++rx) {
            for (float ry = 0; ry < grainH; ++ry) {

                float temp = rx / grainW;
                float humi = (grainH - ry) / grainH;

                float dist;
                Biome *bio = Biome::lookup(temp, humi, &dist);

                if (dist > 0.1)
                    continue;

                auto &cent = bioCenters[bio->getRegistryId()];
                if (cent.first == 0 || dist < cent.first) {  // lazy. cent.first==0 means not-found/auto-created.
                    cent.first = dist;
                    cent.second = {rx * grain, ry * grain};
                }

                vec3 color = Biomes::color(bio);

                Gui::drawRect(x + rx * grain, y + ry * grain, grain, grain, glm::vec4(color, 1.0));

                if (Gui::isCursorOver(x + rx * grain, y + ry * grain, grain, grain)) {
                    Gui::drawString(x, y - 32,
                                    Strings::fmt("{} T{}({}'C) H{}\nT{}({}'C) H{}",
                                                 bio->m_Name, bio->m_Temperature,
                                                 Biome::Temperature::toCelsius(bio->m_Temperature), bio->m_Humidity,
                                                 temp, Biome::Temperature::toCelsius(temp), humi),
                                    glm::vec4(color, 1.0));
                }
            }
        }

        for (auto &it : bioCenters) {
            const Biome* bio = Biome::REGISTRY.get(it.first);
            glm::vec2 p = it.second.second;
            glm::vec3 oriCol = Biomes::color(bio);
            glm::vec4 col = glm::vec4(Colors::luminance(oriCol) > 0.65 ? oriCol - 0.3f : oriCol + 0.5f, 1.0);

            Gui::drawRect(x + p.x - 1, y + p.y - 1, 3, 3, Colors::WHITE);
            Gui::drawString(x + p.x, y + p.y, Strings::fmt("{}", bio->m_Name),
                            col,
                            13, {-0.5, -0.5}, false);
        }
    }


    void drawAudioFreq()
    {

        float x = 100, y = Gui::maxHeight() - 64;
        float w = Gui::maxWidth() - 2*x, h = 200;
        Gui::drawRect(x,y,w, 1, Colors::WHITE);

        int16_t* buf;
        int nFFT = 512;
        int nFFT_2 = nFFT / 2;

        size_t numSampled = AudioEngine::sampleCapture(&buf, nFFT);
        if (!numSampled) {
            Log::info("No audio sampled.");
            return;
        }

        // FFT Spectrum Frequency

        std::vector<std::complex<float>> tFreq(nFFT);
        for (int i = 0; i < nFFT; ++i) {
            int16_t val_raw = buf[i];
            float f = (float)val_raw / 32768.0f;

            tFreq[i].real(f);
            tFreq[i].imag(0);
        }
        std::vector<std::complex<float>> fft = Loader::fft_1d(tFreq);


        float sampFreq = (float)Ethertia::getAudioEngine()->m_CaptureSampleRate;

        for (int i = 0; i < nFFT_2; ++i)
        {
            float re = fft[i].real(), im = fft[i].imag();
            float magnitude = std::sqrt(re*re + im*im);

            // FFT_idx to Freq_bin: i * Fs / nFFT
            float freqbin = i * sampFreq / 2.0 / nFFT_2;

            float lx = x + w*(freqbin / (sampFreq / 2.0));

            float dB = 10.0 * std::log10(re*re + im*im);//20.0f * std::log10(magnitude);

            Gui::drawRect(lx, y, 2, -(magnitude * h), Colors::WHITE);

            if (i % int(nFFT_2/16) == 0 && i != 0) {
                Gui::drawString(lx, y,
                                freqbin < 1000 ? std::to_string(Mth::floor_dn(freqbin, 1)) : Strings::fmt("{}k", Mth::floor_dn(
                                        freqbin / 1000, 1)),
                                Colors::WHITE);
            }

        }
        if (Gui::isCursorOver(x,y-h,w,h*2))
        {
            int i = int(((Gui::cursorX() - x) / w) * nFFT_2);
            float freqbin = i * sampFreq / 2.0 / nFFT_2;

            Gui::drawString(Gui::cursorX(), Gui::cursorY()-16,
                            Strings::fmt("{} Hz", freqbin));
            Gui::drawRect(Gui::cursorX(), y, 1, -h, Colors::GREEN_DARK);
        }

    }

};

#endif //ETHERTIA_GUISCREENMAINMENU_H
