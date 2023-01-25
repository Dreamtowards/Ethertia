//
// Created by Dreamtowards on 2022/8/26.
//

#ifndef ETHERTIA_CHUNKLOADER_H
#define ETHERTIA_CHUNKLOADER_H

#include <iostream>
#include <fstream>
#include <utility>
#include <string>

#include <nbt/nbt_tags.h>

#include <ethertia/util/Strings.h>

#define _ChunkLoader_Disable

class NBT {
public:
    static nbt::tag_list vec3(glm::vec3 v) {
        return nbt::tag_list({v.x, v.y, v.z});
    }
    static glm::vec3 vec3(nbt::tag_list& l) {
        return glm::vec3((float)l.at(0), (float)l.at(1), (float)l.at(2));
    }
    static glm::vec3 vec3(nbt::value& l) { return vec3(l.as<nbt::tag_list>()); }
};

class ChunkLoader
{
public:

    std::string m_ChunkDir;

    std::unordered_map<glm::vec3, std::fstream*> m_OpendFiles;

    ChunkLoader(std::string savedir) : m_ChunkDir(std::move(savedir)) {
        assert(!Loader::fileExists(savedir));
    }



    void loadWorldInfo(World& world) {
        std::ifstream file(_FileWorldInfo(), std::ios::in);
        if (!file) return;

        auto tagWorld = nbt::io::read_compound(file).second;

        world.m_Name = (std::string)tagWorld->at("Name");
        world.m_Seed = (int64_t)tagWorld->at("Seed");
        world.m_DayTime = (float)tagWorld->at("DayTime");
        world.m_InhabitedTime = (float)tagWorld->at("InhabitedTime");

        file.close();
    }

    void saveWorldInfo(const World& world) const {
        std::ofstream file(Loader::ensureFileParentDirsReady(_FileWorldInfo()), std::ios::out);

        nbt::tag_compound tagWorld;
        tagWorld.put("Name", world.m_Name);
        tagWorld.put("Seed", (int64_t)world.m_Seed);
        tagWorld.put("DayTime", world.m_DayTime);
        tagWorld.put("InhabitedTime", world.m_InhabitedTime);
        tagWorld.put("SavedTime", (int64_t)Timer::timestampMillis());

        nbt::io::write_tag("WorldInfo", tagWorld, file);
        file.close();
    }



    Chunk* loadChunk(glm::vec3 chunkpos, World* world) {
        std::ifstream file(_ChunkFile(chunkpos), std::ios::in);
        if (!file) return nullptr;

        Chunk* chunk = new Chunk(chunkpos, world);

        std::unique_ptr<nbt::tag_compound> tagChunk = nbt::io::read_compound(file).second;
        // "SavedTime" not need to be load yet.

        assert(NBT::vec3(tagChunk->at("ChunkPos")) == chunkpos);
        chunk->m_CreatedTime = (int64_t)tagChunk->at("CreatedTime");
        chunk->m_InhabitedTime = (float)tagChunk->at("InhabitedTime");
        chunk->m_Populated = (bool)tagChunk->at("Populated");

        {
            int8_t* cells = &tagChunk->at("Cells").as<nbt::tag_byte_array>().at(0);

            for (int rx = 0; rx < 16; ++rx) {
                for (int ry = 0; ry < 16; ++ry) {
                    for (int rz = 0; rz < 16; ++rz) {
                        int idx = (rx*256 + ry*16 + rz) * (4+1);
                        Cell& c = chunk->getCell(rx,ry,rz);
                        uint8_t u8Id = cells[idx];
                        c.mtl = Material::REGISTRY.getOrderEntry(u8Id);
                        std::memcpy(&c.density, &cells[idx+1], 4);
                    }
                }
            }
        }

        file.close();
        return chunk;
    }

    void saveChunk(Chunk* chunk) {
        std::ofstream file(Loader::ensureFileParentDirsReady(_ChunkFile(chunk->position)), std::ios::out);

        nbt::tag_compound tagChunk;
        tagChunk.put("ChunkPos", NBT::vec3(chunk->position));
        tagChunk.put("CreatedTime", (int64_t)chunk->m_CreatedTime);
        tagChunk.put("SavedTime", (int64_t)Timer::timestampMillis());
        tagChunk.put("InhabitedTime", chunk->m_InhabitedTime);
        tagChunk.put("Populated", chunk->m_Populated);

        // Palette? not need yet, not String Id yet.
        {
            // Uniform Grids Cells.
            static const size_t CELL_SIZE = 1+4,
                                CELLS_ALL_SIZE = 4096 * CELL_SIZE;
            static int8_t cells[CELLS_ALL_SIZE];

            for (int rx = 0; rx < 16; ++rx) {
                for (int ry = 0; ry < 16; ++ry) {
                    for (int rz = 0; rz < 16; ++rz) {
                        int idx = (rx*256 + ry*16 + rz) * CELL_SIZE;
                        const Cell& c = chunk->getCell(rx,ry,rz);
                        cells[idx] = Material::REGISTRY.getOrderId(c.mtl);
                        std::memcpy(&cells[idx+1], &c.density, 4);
                    }
                }
            }

            tagChunk.put("Cells", nbt::tag_byte_array(std::vector<int8_t>(cells, cells+CELLS_ALL_SIZE)));
        }

        nbt::io::write_tag("Chunk", tagChunk, file);

    }

    // .vst Volume Store
    [[nodiscard]] std::string _ChunkFile(glm::vec3 chunkpos) const {
        assert(Chunk::validchunkpos(chunkpos));
        return Strings::fmt("{}/chunks/{}.{}.{}.vst", m_ChunkDir, chunkpos.x, chunkpos.y, chunkpos.z);
    }

    std::string _FileWorldInfo() const {
        return Strings::fmt("{}/world.dat", m_ChunkDir);
    }

};

#endif //ETHERTIA_CHUNKLOADER_H
