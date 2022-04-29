//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_RENDERER_H
#define ETHERTIA_RENDERER_H

// Marker.
class Renderer {

public:

    static const char** _GenArrayNames(const char* namep, uint n) {
        const char** arr = new const char*[n];
        uint baselen = strlen(namep)+3;  // +2: brackets, +1: \0.
        for (int i = 0; i < n; ++i) {
            char* ch = new char[baselen+3];  // +3 assume idx <= 999.
            sprintf(ch, namep, i);
            arr[i] = ch;
        }
        return arr;
    }
};


#endif //ETHERTIA_RENDERER_H
