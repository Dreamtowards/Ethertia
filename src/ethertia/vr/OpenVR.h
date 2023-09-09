////
//// Created by Dreamtowards on 2023/2/8.
////
//
//#ifndef ETHERTIA_OPENVR_H
//#define ETHERTIA_OPENVR_H
//
//#include <openvr.h>
//
//class OpenVR
//{
//public:
//
//    static bool init()
//    {
//        Log::info("VR HMD_Present: {}, RtInstalled: {}",
//                  vr::VR_IsHmdPresent(),
//                  vr::VR_IsRuntimeInstalled());  // vr::VR_GetRuntimePath()
//
//
//
//        vr::EVRInitError err;
//        vr::IVRSystem* m_HMD = vr::VR_Init(&err, vr::EVRApplicationType::VRApplication_Scene);
//
//        if (err) {
//            Log::warn("Failed to init OpenVR: ", vr::VR_GetVRInitErrorAsEnglishDescription(err));
//            return false;
//        }
//
//        Log::info("VR driver: {}, display: {}, model: {}",
//                  get_tracked_device_string(m_HMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String),
//                  get_tracked_device_string(m_HMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String),
//                  get_tracked_device_string(m_HMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_ModelNumber_String));
//
//
//
//        return true;
//    }
//
//
//
//    static inline std::string get_tracked_device_string(vr::IVRSystem * pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError * peError = NULL)
//    {
//        uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, nullptr, 0, peError);
//        if (unRequiredBufferLen == 0)
//            return "";
//
//        char *pchBuffer = new char[unRequiredBufferLen];
//        unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
//        std::string sResult = pchBuffer;
//        delete[] pchBuffer;
//        return sResult;
//    }
//};
//
//#endif //ETHERTIA_OPENVR_H
