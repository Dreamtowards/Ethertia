//
// Created by Dreamtowards on 2023/3/19.
//

#ifndef ETHERTIA_VULKANINTL_H
#define ETHERTIA_VULKANINTL_H

class Vulkan
{
public:

    static void Init(void* glfwWindow);

    static void Destroy();

    static void DrawFrame();

    static void RequestRecreateSwapchain();

};

#endif //ETHERTIA_VULKANINTL_H
