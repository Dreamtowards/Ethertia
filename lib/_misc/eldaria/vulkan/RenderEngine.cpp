//
// Created by Dreamtowards on 2023/4/11.
//

#include "vkx.h"


class RenderEngine
{
    static void Init();

    static void Destroy();
};


void RenderEngine::Init()
{
    vkx::Instance* instance = new vkx::Instance();

}
