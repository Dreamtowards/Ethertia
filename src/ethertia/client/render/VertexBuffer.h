//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_VERTEXBUFFER_H
#define ETHERTIA_VERTEXBUFFER_H

#include <vector>
#include <ethertia/util/Log.h>

// 3d positions.
class VertexBuffer
{
public:
    std::vector<float> positions{};
    std::vector<float> textureCoords{};
    std::vector<float> normals{};
    int i = 0;
    VertexBuffer() {
        i = 1;
    }

    void addpos(float x, float y, float z) {
        positions.push_back(x);
        positions.push_back(y);
        positions.push_back(z);
//        Log::info("AddPos. size: "+std::to_string(positions.size())+", i: "+std::to_string(i));
        i++;
    }

    void adduv(float x, float y) {
        textureCoords.push_back(x);
        textureCoords.push_back(y);
    }

    void addnorm(float x, float y, float z) {
        normals.push_back(x);
        normals.push_back(y);
        normals.push_back(z);
    }

    size_t vertexCount() const {
        return positions.size() / 3;
    }

    void dbg_str() {
        Log::info("VBuf: ");
    }

    ~VertexBuffer() {
        Log::info("VertexBuffer Deleted. pos: "+std::to_string(positions.size())+", vcount: ("+std::to_string(vertexCount())+", pos-added: "+std::to_string(i));
    }
};

#endif //ETHERTIA_VERTEXBUFFER_H
