//
// Created by Dreamtowards on 2023/1/19.
//

#ifndef ETHERTIA_GLSTATE_H
#define ETHERTIA_GLSTATE_H

class GlState
{
public:

//    inline static void enableBlend() {
//        glEnable(GL_BLEND);
//    }
//    inline static void blendFunc(int sf, int df) {
//        glBlendFunc(sf,df);
//    }

    enum Blend {
        ALPHA = 0,
        ADD = 1,
        SUBSTRACT,
        MULTIPLY,
        DODGE,
        BURN,
        SCREEN,
        OVERLAY,
        REPLACE
    };

    static void blendMode(Blend mode)
    {
        // disable alpha test, i.e. no discard due little alpha.

        if (mode == Blend::REPLACE) {
            glDisable(GL_BLEND);
        } else {
            glEnable(GL_BLEND);
        }

        switch (mode) {
            case ALPHA:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                break;
            case ADD:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                break;
            case SUBSTRACT:
                glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
                break;
            case MULTIPLY:
                glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
                break;
            case DODGE:
                glBlendFunc(GL_ONE, GL_ONE);
                break;
            case BURN:
                glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
                break;
            case SCREEN:
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
                break;
            case OVERLAY:
                glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
                break;
            default:
                assert(false);
        }
    }


};

#endif //ETHERTIA_GLSTATE_H
