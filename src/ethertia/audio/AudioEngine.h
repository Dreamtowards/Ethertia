//
// Created by Dreamtowards on 2022/4/29.
//

#ifndef ETHERTIA_AUDIOENGINE_H
#define ETHERTIA_AUDIOENGINE_H

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include <ethertia/util/Log.h>

class AudioEngine
{
public:

    ALCdevice* m_Device = nullptr;
    ALCcontext* m_Context = nullptr;

    AudioEngine()
    {
        const char* deviceName = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);

        m_Device = alcOpenDevice(deviceName);
        if (!m_Device)
            throw new std::runtime_error("failed open al device.");

        m_Context = alcCreateContext(m_Device, nullptr);
        if (!m_Context)
            throw new std::runtime_error("failed create al context.");

        alcMakeContextCurrent(m_Context);

        Log::info("Init AL_{} | {} @{} .{} / Capture: {} ",
                  alGetString(AL_VERSION),
                  deviceName,
                  alcGetString(nullptr, ALC_DEFAULT_ALL_DEVICES_SPECIFIER),
                  _infoOutputMode(m_Device),
                  alcGetString(nullptr, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER));

#define FUNCTION_CAST(T, ptr) (T)(ptr)
        LPALCGETSTRINGISOFT alcGetStringiSOFT;
        alcGetStringiSOFT = FUNCTION_CAST(LPALCGETSTRINGISOFT,
                                          alcGetProcAddress(m_Device, "alcGetStringiSOFT"));

        ALCint numHRTFs;
        alcGetIntegerv(m_Device, ALC_NUM_HRTF_SPECIFIERS_SOFT, 2, &numHRTFs);
        for (int i = 0; i < numHRTFs; ++i) {
            Log::info("HRTF: ", alcGetStringiSOFT(m_Device, ALC_HRTF_SPECIFIER_SOFT, i));
        }




        bool eax2 = alIsExtensionPresent("EAX2.0");
        Log::info("EAX2: ", eax2);


    }

    ~AudioEngine()
    {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_Context);
        alcCloseDevice(m_Device);
    }






    static const char* _infoOutputMode(ALCdevice* device) {
        const char* modename = "(error)";
        if(alcIsExtensionPresent(device, "ALC_SOFT_output_mode")) {
            ALCenum mode = 0;
            alcGetIntegerv(device, ALC_OUTPUT_MODE_SOFT, 1, &mode);

            switch(mode) {
                case ALC_ANY_SOFT: modename = "Unknown / unspecified"; break;
                case ALC_MONO_SOFT: modename = "Mono"; break;
                case ALC_STEREO_SOFT: modename = "Stereo (unspecified encoding)"; break;
                case ALC_STEREO_BASIC_SOFT: modename = "Stereo (basic)"; break;
                case ALC_STEREO_UHJ_SOFT: modename = "Stereo (UHJ)"; break;
                case ALC_STEREO_HRTF_SOFT: modename = "Stereo (HRTF)"; break;
                case ALC_QUAD_SOFT: modename = "Quadraphonic"; break;
                case ALC_SURROUND_5_1_SOFT: modename = "5.1 Surround"; break;
                case ALC_SURROUND_6_1_SOFT: modename = "6.1 Surround"; break;
                case ALC_SURROUND_7_1_SOFT: modename = "7.1 Surround"; break;
            }
        }
        return modename;
    }
};

#endif //ETHERTIA_AUDIOENGINE_H
