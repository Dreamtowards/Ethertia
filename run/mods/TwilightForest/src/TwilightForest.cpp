//
// Created by Dreamtowards on 2023/1/31.
//

#include <iostream>

//#include <ethertia/mod/Mod.h>
//#include <ethertia/util/Log.h>

// this is the exclusive entrance of a mod. called when system early boot.
// but please don't do lot work there, just as few as possible. usually register some listeners.

extern "C"
void init()
{


    std::cout << "Twilight Forest Initialized\n";
}