//
// Created by Dreamtowards on 2023/3/19.
//

#ifndef ETHERTIA_IMGUI_H
#define ETHERTIA_IMGUI_H

class GLFWwindow;

typedef struct VkCommandBuffer_T* VkCommandBuffer;

class Imgui
{
public:
    inline static bool
        w_Settings = false,
        w_Viewport = false,
        w_EntityInsp = false,
        w_EntityList = false,
        w_Console = false,
        w_ImguiDemo = false,
        w_Toolbar = false;
    // w_ShaderInsp


    static void Init(GLFWwindow* glfwWindow);

    static void Destroy();

    static void RenderGUI(VkCommandBuffer cmdbuf);


};

#endif //ETHERTIA_IMGUI_H
