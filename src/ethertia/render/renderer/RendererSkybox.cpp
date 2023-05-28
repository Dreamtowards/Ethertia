//
// Created by Dreamtowards on 2023/5/24.
//

#include <nlohmann/json.hpp>

namespace RendererSkybox
{
    VkPipeline g_Pipeline;


    void init()
    {


        nlohmann::json skyConf = nlohmann::json::parse((std::span<const char>)Loader::loadFile("misc/sky/skybox.json"));



    }
}