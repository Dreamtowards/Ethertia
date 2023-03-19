//
// Created by Dreamtowards on 2023/3/19.
//

#ifndef ETHERTIA_LOADER_H
#define ETHERTIA_LOADER_H

#include <eldaria/BitmapImage.h>

//class VertexData
//{
//public:
//
//    std::vector<glm::vec3> m_Positions;
//    std::vector<glm::vec2> m_TexCoords;
//    std::vector<glm::vec3> m_Normals;
//};

class Loader
{
public:

    // load entire file. filename could be absolute-path or relative-path.
    static std::vector<char> loadFile(const std::string& filename);


    // backend: stb_image. supports PNG, JPG, GIF, TGA, BMP, HDR, PSD, PIC, PPM/PGM.
    static BitmapImage loadPNG(const std::string& filename);
    // fixme: return ptr? std::shared_ptr? or just object?





};

#endif //ETHERTIA_LOADER_H
