//
// Created by Dreamtowards on 2022/5/24.
//

#include "Eth.h"

#include <ethertia/client/Ethertia.h>

Camera* Eth::getCamera() {
    return Ethertia::getCamera();
}

RenderEngine* Eth::getRenderEngine() {
    return Ethertia::getRenderEngine();
}

Window* Eth::getWindow() {
    return Ethertia::getWindow();
}