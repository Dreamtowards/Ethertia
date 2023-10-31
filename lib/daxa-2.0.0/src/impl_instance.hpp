#pragma once

#include <daxa/instance.hpp>

#include "impl_core.hpp"

namespace daxa
{
    struct ImplInstance final : ManagedSharedState
    {
        InstanceInfo info;
        VkInstance vk_instance = {};
        VkDebugUtilsMessengerEXT vk_debug_utils_messenger = {};
        bool enable_debug_names = true;

        explicit ImplInstance(InstanceInfo a_info);
        virtual ~ImplInstance() override final;
    };
} // namespace daxa
