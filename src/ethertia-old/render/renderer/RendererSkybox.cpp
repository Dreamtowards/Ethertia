//
// Created by Dreamtowards on 2023/5/24.
//

#include <nlohmann/json.hpp>

namespace RendererSkybox
{
    VkPipeline g_Pipeline;

    struct Skybox
    {
        bool Rotate = true;
        float RotateSpeed = 1.0f;  // how many cycle(360') per day(24h).
        glm::vec3 RotateAxis = {0, 0, 1};

        float FadeInBegin = 0;
        float FadeInEnd = 0;

        float FadeOutBegin = 0;
        float FadeOutEnd = 0;

        std::string CubeMapImageFilename;

        int BlendMode;



    };
    std::vector<Skybox> g_Skyboxes;

    void loadConf()
    {
        nlohmann::json skyConf = nlohmann::json::parse((std::span<const char>)Loader::loadAssets("misc/sky/skybox.json"));

        for (nlohmann::json& skyInfo : skyConf)
        {
            Log::info("SkyFilename: ", skyInfo["source"]);
        }
    }


    void init()
    {


        loadConf();


    }
}