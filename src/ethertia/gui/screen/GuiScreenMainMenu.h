//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUISCREENMAINMENU_H
#define ETHERTIA_GUISCREENMAINMENU_H

#include "../GuiSlider.h"
#include "../GuiText.h"
#include "../GuiCollection.h"
#include "../GuiAlign.h"
#include "../GuiStack.h"
#include "../GuiButton.h"

#include <ethertia/render/Camera.h>
#include <ethertia/world/Biome.h>


#include "dj-fft/dj_fft.h"

class GuiScreenMainMenu : public GuiCollection
{
public:
    inline static GuiScreenMainMenu* INST;

    GuiScreenMainMenu()
    {
        addDrawBackground(Colors::WHITE80);
        setWidthHeight(Inf, Inf);

        GuiStack* vstack = new GuiStack(GuiStack::D_VERTICAL, 16);

        GuiButton* btnSingleplayer = new GuiButton("Singleplayer");
        vstack->addGui(btnSingleplayer);
        btnSingleplayer->addOnClickListener([](auto)
        {
            Ethertia::loadWorld();
        });

        GuiButton* btnMultiplayer = new GuiButton("Multiplayer");
        vstack->addGui(btnMultiplayer);
        btnMultiplayer->addOnClickListener([](auto) {
            Loader::openURL(Loader::showInputBox("URL", "File, Folder, or URL", ""));
        });

        GuiButton* btnSettings = new GuiButton("Settings");
        vstack->addGui(btnSettings);
        btnSettings->addOnClickListener([](auto) {

        });

        GuiButton* btnExit = new GuiButton("Terminate");
        vstack->addGui(btnExit);
        btnExit->addOnClickListener([](auto)
        {
            Ethertia::shutdown();
        });

        addGui(new GuiAlign(0.5, 0.5, vstack));  // x 0.12
    }

    void implDraw() override
    {

        drawAudioFreq();

        Gui::drawString(0, Gui::maxHeight(), "0 mods loaded.\n"
                                                "Ethertia "+Ethertia::Version::name(), Colors::WHITE60, 16,{0.0f, -1.0f});


        Gui::drawString(Gui::maxWidth(), Gui::maxHeight(), "Seasonally Excellent Dev: Eldrine Le Prismarine.\n"
                                                  "Copyright Elytra Corporation. Do not distribute!", Colors::WHITE60, 16, {1.0, -1.0f});


//        Biome::find()

        {
            // Biome Distribution of Temperature and Humidity

            float x = 10, y = 100, w = 200, h = 200;

            for (int rx = 0; rx < w; ++rx) {
                for (int ry = 0; ry < h; ++ry) {

                    float temp = Mth::lerp(rx / w, -10, 30);
                    float humi;

                }
            }

        }



    }

    void drawAudioFreq()
    {

        float x = 100, y = Gui::maxHeight() - 64;
        float w = Gui::maxWidth() - 2*x, h = 200;
        Gui::drawRect(x,y,w, 1, Colors::WHITE);

        int16_t* buf;
        int nFFT = 1024;
        int nFFT_2 = nFFT / 2;

        Ethertia::getAudioEngine()->sampleCapture(&buf, nFFT);

        // FFT Spectrum Frequency

        std::vector<std::complex<float>> tFreq(nFFT);
        for (int i = 0; i < nFFT; ++i) {
            int16_t val_raw = buf[i];
            float f = (float)val_raw / 32768.0f;

            tFreq[i].real(f);
            tFreq[i].imag(0);
        }
        auto fft = dj::fft1d(tFreq, dj::fft_dir::DIR_BWD);


        float sampFreq = Ethertia::getAudioEngine()->m_CaptureSampleRate;

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
