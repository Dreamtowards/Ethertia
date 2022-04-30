//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_TESTEVENTBUS_H
#define ETHERTIA_TESTEVENTBUS_H

#include <ethertia/event/EventBus.h>

#include "BenchmarkTimer.h"

//#include "ethertia/util/testing/TestEventBus.h"
//    testEventBus();

void funcptr(int* e) {

}

int testEventBus() {

    EventBus eventbus;



    eventbus.listen(funcptr);

    eventbus.listen([](int* i){

    });

    for (int i = 0; i < 6; ++i)
    {
        BenchmarkTimer tmr("Post 1k");

        for (int i = 0; i < 1000; ++i) {
            eventbus.post(&i);
        }

    }


    return 0;
}

#endif //ETHERTIA_TESTEVENTBUS_H
