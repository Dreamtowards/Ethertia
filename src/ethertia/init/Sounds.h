//
// Created by Dreamtowards on 2023/2/23.
//

#ifndef ETHERTIA_SOUNDS_H
#define ETHERTIA_SOUNDS_H

#include <ethertia/audio/AudioBuffer.h>

class Sounds
{
public:

#define DECL_Sound(varname, file) inline static AudioBuffer* varname = nullptr;


    DECL_Sound(CELL_BREAK, "sounds/break.ogg");
    DECL_Sound(CELL_PLACE, "sounds/place.ogg");

    DECL_Sound(GUI_CLICK, "sounds/gui/click.ogg");
    DECL_Sound(GUI_HOVER, "sounds/gui/hover.ogg");


};

#endif //ETHERTIA_SOUNDS_H
