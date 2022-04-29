//
// Created by Dreamtowards on 2022/4/29.
//

#include "Gui.h"
#include <ethertia/client/Ethertia.h>

float Gui::maxWidth() {
    return Ethertia::getWindow()->getWidth();
}

float Gui::maxHeight() {
    return Ethertia::getWindow()->getHeight();
}