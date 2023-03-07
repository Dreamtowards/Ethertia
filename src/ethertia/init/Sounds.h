//
// Created by Dreamtowards on 2023/2/23.
//

#ifndef ETHERTIA_SOUNDS_H
#define ETHERTIA_SOUNDS_H

#include <ethertia/audio/AudioBuffer.h>

class Sounds
{
public:

    inline static AudioSource* AS_GUI = nullptr;


#define DECL_Sound(varname, file) inline static AudioBuffer* varname = _LaterLoad(file);

    inline static std::vector<std::pair<std::string, AudioBuffer*>> _LaterLoadList;
    static AudioBuffer* _LaterLoad(const std::string& filepath) {
        AudioBuffer* buf = new AudioBuffer(false);  // tmp create
        _LaterLoadList.push_back({ filepath, buf });
        return buf;
    }

    static void load()
    {
        BENCHMARK_TIMER;
        Log::info("Loading sounds ({})..\1", _LaterLoadList.size());

        AS_GUI = new AudioSource();
        AS_GUI->looping(false);


        for (auto& it : _LaterLoadList)
        {
            Log::info("Load Sound: {}", it.first);
            *it.second = *Loader::loadOGG(Loader::loadFile(Loader::fileResolve(it.first)));
            assert(it.second->m_BufferId > 0);

            AudioEngine::checkAlError("Load OGG File");
        }
    }


    DECL_Sound(DIG_STONE, "sounds/dig/stone1.ogg");
//    DECL_Sound(CELL_PLACE, "sounds/place.ogg");

    DECL_Sound(GUI_CLICK, "sounds/gui/click_stereo.ogg");
//    DECL_Sound(GUI_HOVER, "sounds/gui/hover.ogg");


};

#endif //ETHERTIA_SOUNDS_H
