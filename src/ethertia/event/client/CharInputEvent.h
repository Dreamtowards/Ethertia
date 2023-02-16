//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_CHARINPUTEVENT_H
#define ETHERTIA_CHARINPUTEVENT_H

class CharInputEvent {

    uint _char;

public:
    CharInputEvent(uint _char) : _char(_char) {}

    uint getChar() {
        return _char;
    }

};

#endif //ETHERTIA_CHARINPUTEVENT_H
