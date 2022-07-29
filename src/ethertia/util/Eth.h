//
// Created by Dreamtowards on 2022/5/24.
//

#ifndef ETHERTIA_ETH_H
#define ETHERTIA_ETH_H

// Helper for Headers.

class Camera;
class RenderEngine;
class Window;

class Eth
{
public:
    static Camera* getCamera();

    static RenderEngine* getRenderEngine();

    static Window* getWindow();
};

#endif //ETHERTIA_ETH_H
