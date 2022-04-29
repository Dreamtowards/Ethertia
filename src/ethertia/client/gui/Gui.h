//
// Created by Dreamtowards on 2022/4/28.
//

#ifndef ETHERTIA_GUI_H
#define ETHERTIA_GUI_H

class Gui
{
    float width;
    float height;

public:
    float getWidth() const {
        return width;
    }
    float getHeight() const {
        return height;
    }

    static float maxWidth();
    static float maxHeight();

};

#endif //ETHERTIA_GUI_H
