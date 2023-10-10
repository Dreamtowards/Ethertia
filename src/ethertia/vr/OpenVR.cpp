
#include "OpenVR.h"


#include <ethertia/util/Assert.h>
#include <ethertia/util/Log.h>


static std::string _GetTrackedDeviceString(vr::IVRSystem* pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError* peError = NULL)
{
    uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, nullptr, 0, peError);
    if (unRequiredBufferLen == 0)
        return "";

    char* pchBuffer = new char[unRequiredBufferLen];
    unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
    std::string sResult = pchBuffer;
    delete[] pchBuffer;
    return sResult;
}


static vr::IVRSystem* g_VRContext = nullptr;


void OpenVR::Init()
{
    Log::info("VR IsHmdPresent: {}, IsRuntimeInstalled: {}, RuntimePath: {}",
        vr::VR_IsHmdPresent(), vr::VR_IsRuntimeInstalled(), "vr::VR_GetRuntimePath()");

    vr::EVRInitError err;
    g_VRContext = vr::VR_Init(&err, vr::EVRApplicationType::VRApplication_Scene);

    if (err)
        ET_ERROR("Failed to init OpenVR: {}", vr::VR_GetVRInitErrorAsEnglishDescription(err));

    for (int device = vr::k_unTrackedDeviceIndex_Hmd; device < vr::k_unMaxTrackedDeviceCount; ++device)
    {
        if (g_VRContext->IsTrackedDeviceConnected(device)) // Check if that device is connected 
        {
            vr::ETrackedDeviceClass dclass = g_VRContext->GetTrackedDeviceClass(device);  // GetTrackedDeviceClassString
            //td_name = vr_context->GetStringTrackedDeviceProperty(device, Prop_TrackingSystemName_String);
//
            //print "Type: " + td_class + ", " + "Name: " + td_name;
        }
    }

    Log::info("VR driver: {}, display: {}, model: {}",
        _GetTrackedDeviceString(g_VRContext, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String),
        _GetTrackedDeviceString(g_VRContext, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String),
        _GetTrackedDeviceString(g_VRContext, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_ModelNumber_String));

}