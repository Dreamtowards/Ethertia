//
// Created by Dreamtowards on 10/10/2023.
//

#include "OpenXR.h"

#include <vector>


vrx::_InstanceContext& vrx::ctx() {
    static _InstanceContext inst{};
    return inst;
}

static XrInstance _CreateXrInstance(bool enableDebugMessenger, XrDebugUtilsMessengerEXT& out_DebugMessenger)
{
    std::vector<const char*> extensions = {
            XR_EXT_DEBUG_UTILS_EXTENSION_NAME
    };

    XrInstanceCreateInfo instInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    instInfo.enabledExtensionCount = extensions.size();
    instInfo.enabledExtensionNames = extensions.data();
    instInfo.applicationInfo = {
            .applicationName = "XR Application Name",
            .apiVersion = XR_CURRENT_API_VERSION
    };
    XrInstance xrInstance;
    xrCreateInstance(&instInfo, &xrInstance);

    // Init DebugUtilsMessenger
    if (enableDebugMessenger)
    {
        XrDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = { XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        debugMessengerInfo.messageTypes =
                XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
                XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
                XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;
        debugMessengerInfo.messageSeverities =
                XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
                XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessengerInfo.userCallback = [](XrDebugUtilsMessageSeverityFlagsEXT severity, XrDebugUtilsMessageTypeFlagsEXT types, const XrDebugUtilsMessengerCallbackDataEXT *msg, void* user_data) {
            // Print the debug message we got! There's a bunch more info we could
            // add here too, but this is a pretty good start, and you can always
            // add a breakpoint this line!
            printf("%s: %s\n", msg->functionName, msg->message);

            // Returning XR_TRUE here will force the calling function to fail
            return (XrBool32)XR_FALSE;
        };

        PFN_xrCreateDebugUtilsMessengerEXT _EXT_xrCreateDebugUtilsMessengerEXT;
        xrGetInstanceProcAddr(xrInstance, "xrCreateDebugUtilsMessengerEXT",  (PFN_xrVoidFunction*)(&_EXT_xrCreateDebugUtilsMessengerEXT ));
        _EXT_xrCreateDebugUtilsMessengerEXT(xrInstance, &debugMessengerInfo, &out_DebugMessenger);
    }
}



void vrx::Init()
{
    auto& i = vrx::ctx();

    i.Instance =
    _CreateXrInstance(true, i.DebugMessenger);

    XrInstance xrInstance = i.Instance;
    XrSystemId& xrSystemId = i.SystemId;
    XrSession& xrSession = i.Session;

    XrFormFactor            app_config_form = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    // Request a form factor from the device (HMD, Handheld, etc.)
    XrSystemGetInfo systemInfo = { XR_TYPE_SYSTEM_GET_INFO };
    systemInfo.formFactor = app_config_form;
    xrGetSystem(xrInstance, &systemInfo, &xrSystemId);



    // A session represents this application's desire to display things! This is where we hook up our graphics API.
    // This does not start the session, for that, you'll need a call to xrBeginSession, which we do in openxr_poll_events
    XrSessionCreateInfo sessionInfo = { XR_TYPE_SESSION_CREATE_INFO };
    //sessionInfo.next     = &binding;
    sessionInfo.systemId = xrSystemId;
    xrCreateSession(xrInstance, &sessionInfo, &xrSession);


    // OpenXR uses a couple different types of reference frames for positioning content, we need to choose one for
    // displaying our content! STAGE would be relative to the center of your guardian system's bounds, and LOCAL
    // would be relative to your device's starting location. HoloLens doesn't have a STAGE, so we'll use LOCAL.
    //XrReferenceSpaceCreateInfo ref_space = { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
    //ref_space.poseInReferenceSpace = xr_pose_identity;
    //ref_space.referenceSpaceType   = XR_REFERENCE_SPACE_TYPE_LOCAL;
    //xrCreateReferenceSpace(xr_session, &ref_space, &xr_app_space);
}

void OpenXR::Destroy()
{


    //PFN_xrDestroyDebugUtilsMessengerEXT _EXT_xrDestroyDebugUtilsMessengerEXT;
    //xrGetInstanceProcAddr(xrInstance, "xrDestroyDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)(&_EXT_xrDestroyDebugUtilsMessengerEXT));
    //_EXT_xrDestroyDebugUtilsMessengerEXT(_EXT_DebugMessenger);

    // xrDestroyInstance();
}