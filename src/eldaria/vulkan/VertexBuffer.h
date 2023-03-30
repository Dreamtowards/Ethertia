//
// Created by 56327 on 2023/3/30.
//

#ifndef ETHERTIA_VERTEXBUFFER_H
#define ETHERTIA_VERTEXBUFFER_H

#include "vkh.h"

class VertexBuffer
{
public:
    VkBuffer m_VertexBuffer = nullptr;
    VkDeviceMemory m_VertexBufferMemory = nullptr;

    VkBuffer m_IndexBuffer = nullptr;
    VkDeviceMemory m_IndexBufferMemory = nullptr;

    // used by vkCmdDraw()
    int m_VertexCount = 0;

    // used for Destroy.
    VkDevice m_Device;

    void destroy()
    {
        vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
        vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);

        vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);
        vkFreeMemory(m_Device, m_IndexBufferMemory, nullptr);
    }
};

#endif //ETHERTIA_VERTEXBUFFER_H
