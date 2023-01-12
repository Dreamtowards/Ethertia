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

        Gui::drawString(0, Gui::maxHeight(), "0 mods loaded.\n"
                                                "Ethertia "+Ethertia::Version::name(), Colors::WHITE60, 16,{0.0f, -1.0f});


        Gui::drawString(Gui::maxWidth(), Gui::maxHeight(), "Seasonally Excellent Dev: Eldrine Le Prismarine.\n"
                                                  "Copyright Elytra Corporation. Do not distribute!", Colors::WHITE60, 16, {1.0, -1.0f});



        {
            float x = 100, y = 500;
            float w = 1000, h = 100;
            int16_t* buf;
            size_t samples_act = Ethertia::getAudioEngine()->sampleCapture(&buf);

            Log::info("Samples: {} : {}", samples_act, buf[0]);

            int samples = 512;

            // FFT Spectrum Frequency

//            float* real = new float[samples];
//            float* imag = new float[samples];
//            for (int i = 0; i < samples; ++i) {
//                int16_t val_raw = buf[i];
//                float f = (float)val_raw / 32768.0f;
//                real[i] = f;
//                imag[i] = f;
//            }
//
//            Mth::FFT(real, imag, samples);

//            _FFT::Complex comp[samples];
//            for (int i = 0; i < samples; ++i) {
//                int16_t val_raw = buf[i];
//                float f = (float)val_raw / 32768.0f;
//                comp[i].imag(0);
//                comp[i].real(f);
//            }
//
//            _FFT::CArray arr(comp, samples);
//            _FFT::fft(arr);

            std::vector<std::complex<float>> src(samples);
            for (int i = 0; i < samples; ++i) {
                int16_t val_raw = buf[i];
                float f = (float)val_raw / 32768.0f;

                src[i].real(f);
                src[i].imag(0);
            }

            auto dst = dj::fft1d(src, dj::fft_dir::DIR_BWD);




            Gui::drawRect(x,y,w, 1, Colors::WHITE);

            float col_w = w / samples;
            for (int i = 0; i < samples; ++i) {
                float f = dst[i].real();
//                float f = real[i];

                Gui::drawRect(x+ i*col_w, y, col_w, -f * h, Colors::WHITE);
            }

            Timer::sleep_for(80);

        }



    }

    Gui* buildOptItem(const std::string& name, Gui* g) {

        auto* it = new GuiStack(GuiStack::D_HORIZONTAL);

        GuiText* label = new GuiText(name);


        it->addGui(label);
        it->addGui(g);

        return it;
    }
};

#endif //ETHERTIA_GUISCREENMAINMENU_H
