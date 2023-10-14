//
// Created by Dreamtowards on 10/10/2023.
//

#pragma once

#define XR_USE_GRAPHICS
#include <openxr/openxr.h>

namespace vrx
{
    struct _InstanceContext
    {
        XrInstance Instance;

        XrSystemId SystemId = XR_NULL_SYSTEM_ID;
        XrSession Session;

        bool EnableDebugMessenger = false;
        XrDebugUtilsMessengerEXT DebugMessenger = {};

    };

    _InstanceContext& ctx();

    void Init();


}


class OpenXR
{
public:

    static void Init();

    static void Destroy();

};