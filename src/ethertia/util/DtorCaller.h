//
// Created by Dreamtowards on 2023/2/14.
//

#ifndef ETHERTIA_DTORCALLER_H
#define ETHERTIA_DTORCALLER_H

// use for Scoped push/pop. for Profiler, Framebuffer

class DtorCaller
{
public:

    std::function<void()> fn;

    ~DtorCaller()
    {
        fn();
    }
};

#endif //ETHERTIA_DTORCALLER_H
