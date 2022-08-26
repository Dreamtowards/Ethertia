//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_CHARINPUTEVENT_H
#define ETHERTIA_CHARINPUTEVENT_H

class CharInputEvent {

    u32 _char;

public:
    CharInputEvent(u32 _char) : _char(_char) {}

    u32 getChar() {
        return _char;
    }

};

#endif //ETHERTIA_CHARINPUTEVENT_H
