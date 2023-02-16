//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_CHARINPUTEVENT_H
#define ETHERTIA_CHARINPUTEVENT_H

class CharInputEvent {

    int _char;

public:
    CharInputEvent(int _char) : _char(_char) {}

    int getChar() {
        return _char;
    }

};

#endif //ETHERTIA_CHARINPUTEVENT_H
