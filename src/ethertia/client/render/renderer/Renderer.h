//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_RENDERER_H
#define ETHERTIA_RENDERER_H

#include <ethertia/client/render/shader/ShaderProgram.h>

// Marker.
class Renderer {

public:

    static const char** _GenArrayNames(const std::string& namep, uint n) {
        const char** arr = new const char*[n];
        u32 baselen = namep.length()+3;  // +2: brackets, +1: \0.
        for (int i = 0; i < n; ++i) {
            char* ch = new char[baselen+3];  // +3 assume idx <= 999.
            sprintf(ch, namep.c_str(), i);
            arr[i] = ch;
        }
        return arr;
    }

    static void setShaderMVP(ShaderProgram* shader, glm::mat4 matModel);

    static void setShaderCamPos(ShaderProgram* shader);

};


#endif //ETHERTIA_RENDERER_H
