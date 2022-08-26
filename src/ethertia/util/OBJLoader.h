//
// Created by Dreamtowards on 2022/8/19.
//

#ifndef ETHERTIA_OBJLOADER_H
#define ETHERTIA_OBJLOADER_H

#include <vector>
#include <istream>
#include <string>

#include <ethertia/util/Strings.h>

class OBJLoader
{
public:

    static void loadOBJ(std::stringstream& in, VertexBuffer* vbuf) {

        std::vector<float> objpos;
        std::vector<float> objtex;
        std::vector<float> objnorm;

        bool f_started = false;
        std::string line;
        while (std::getline(in, line))
        {
            std::vector<std::string> c = Strings::splitSpaces(line);

            if (line.find("v ") == 0) {
                assert(c.size() == 4); assert(!f_started);

                objpos.push_back(std::stof(c[1]));
                objpos.push_back(std::stof(c[2]));
                objpos.push_back(std::stof(c[3]));
            } else if (line.find("vt ") == 0) {
                assert(c.size() == 3); assert(!f_started);

                objtex.push_back(std::stof(c[1]));
                objtex.push_back(std::stof(c[2]));
            } else if (line.find("vn ") == 0) {
                assert(c.size() == 4); assert(!f_started);

                objnorm.push_back(std::stof(c[1]));
                objnorm.push_back(std::stof(c[2]));
                objnorm.push_back(std::stof(c[3]));
            } else if (line.find("f ") == 0) {
                assert(c.size() == 4);
                f_started = true;  // once started reading faces, vertex info adding are not allowed

                for (int i = 0; i < 3; ++i) {
                    // vertices:
                    std::vector<std::string> idxs = Strings::split(c[1+i], "/");
                    Log::info("Face{}: {} {} {}", i, idxs[0], idxs[1], idxs[2]);
                    int idx_pos = std::stoi(idxs[0])-1;
                    float* pos_base = &objpos[idx_pos*3];
                    vbuf->addpos(pos_base[0], pos_base[1], pos_base[2]);

                    int idx_tex = std::stoi(idxs[1])-1;
                    float* tex_base = &objtex[idx_tex*2];
                    vbuf->adduv(tex_base[0], tex_base[1]);

                    int idx_norm = std::stoi(idxs[2])-1;
                    float* norm_base = &objnorm[idx_norm*3];
                    vbuf->addnorm(norm_base[0], norm_base[1], norm_base[2]);
                }
            }
        }

    }


};

#endif //ETHERTIA_OBJLOADER_H
