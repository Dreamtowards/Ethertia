//
// Created by Dreamtowards on 2022/4/29.
//

#ifndef ETHERTIA_AUDIOENGINE_H
#define ETHERTIA_AUDIOENGINE_H

#include <OpenAL/alc.h>
#include <OpenAL/al.h>

#include <ethertia/util/Log.h>

class AudioEngine
{
public:

    AudioEngine()
    {
        const char* deviceName = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);

        Log::info("Default Device: {}, Cap: {}", deviceName, alcGetString(nullptr, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER));



        ALCdevice* device = alcOpenDevice(deviceName);
        if (!device)
            return;

        ALCcontext* context = alcCreateContext(device, nullptr);
        if (!context)
            return;

        alcMakeContextCurrent(context);

//        alcGetString(device, ALC_DEVICE_SPECIFIER)


    }
};

#endif //ETHERTIA_AUDIOENGINE_H
