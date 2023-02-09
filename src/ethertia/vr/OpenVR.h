//
// Created by Dreamtowards on 2023/2/8.
//

#ifndef ETHERTIA_OPENVR_H
#define ETHERTIA_OPENVR_H

#include <openvr.h>

class OpenVR
{
public:

    void init()
    {
        vr::EVRInitError err;
        vr::VR_Init(&err, vr::EVRApplicationType::VRApplication_Scene, "");


    }

};

#endif //ETHERTIA_OPENVR_H
