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

class ChunkLoader
{
public:

    class NBT {
    public:
        static nbt::tag_list vec3(glm::vec3 v) {
            return nbt::tag_list({v.x, v.y, v.z});
        }
        static glm::vec3 vec3(nbt::tag_list& l) {
            return glm::vec3((float)l.at(0), (float)l.at(1), (float)l.at(2));
        }
    };

    std::unordered_map<glm::vec3, void*> m_OpendFiles;

    std::string m_ChunkDir;

    ChunkLoader(std::string savedir) : m_ChunkDir(std::move(savedir)) {
        assert(!Loader::fileExists(savedir));
    }

    Chunk* loadChunk(glm::vec3 chunkpos, World* world) {  return nullptr;
//        std::ifstream file(_File(chunkpos), std::ios::in);
//        if (!file)
//            return nullptr;
//
//        Chunk* chunk = new Chunk(chunkpos, world);
//
//        std::unique_ptr<nbt::tag_compound> tagChunk = nbt::io::read_compound(file).second;
//
//        assert(NBT::vec3(tagChunk->at("ChunkPos").as<nbt::tag_list>()) == chunkpos);
//        chunk->m_CreatedTime = (int64_t)tagChunk->at("CreatedTime");
//        // "SavedTime" not need to be load yet.
//        chunk->m_InhabitedTime = (float)tagChunk->at("InhabitedTime");
//        chunk->m_Populated = (bool)tagChunk->at("Populated");
//
//        {
//            int8_t* cells = &tagChunk->at("Cells").as<nbt::tag_byte_array>().at(0);
//
//            for (int rx = 0; rx < 16; ++rx) {
//                for (int ry = 0; ry < 16; ++ry) {
//                    for (int rz = 0; rz < 16; ++rz) {
//                        int idx = (rx*256 + ry*16 + rz) * (4+1);
//                        Cell& c = chunk->getCell(rx,ry,rz);
//                        uint8_t u8Id = cells[idx];
//                        c.mtl = Material::REGISTRY.at(u8Id);
//                        std::memcpy(&c.density, &cells[idx+1], 4);
//                    }
//                }
//            }
//        }
//
//        file.close();
//        return chunk;
    }

    void saveChunk(Chunk* chunk) {  return;
//        std::string filename = _File(chunk->position);
//        std::ofstream file(filename, std::ios::out);
//        Loader::ensureFileParentDirsReady(filename);
//
//        nbt::tag_compound tagChunk;
//        tagChunk.put("ChunkPos", NBT::vec3(chunk->position));
//        tagChunk.put("CreatedTime", (int64_t)chunk->m_CreatedTime);
//        tagChunk.put("SavedTime", (int64_t)Timer::timestampMillis());
//        tagChunk.put("InhabitedTime", chunk->m_InhabitedTime);
//        tagChunk.put("Populated", chunk->m_Populated);
//
//        // Palette? not need yet, not String Id yet.
//        {
//            // Uniform Grids Cells.
//            static const size_t CELL_SIZE = 1+4,
//                                CELLS_ALL_SIZE = 4096 * CELL_SIZE;
//            static int8_t cells[CELLS_ALL_SIZE];
//
//            for (int rx = 0; rx < 16; ++rx) {
//                for (int ry = 0; ry < 16; ++ry) {
//                    for (int rz = 0; rz < 16; ++rz) {
//                        int idx = (rx*256 + ry*16 + rz) * CELL_SIZE;
//                        const Cell& c = chunk->getCell(rx,ry,rz);
//                        cells[idx] = c.mtl;
//                        std::memcpy(&cells[idx+1], &c.density, 4);
//                    }
//                }
//            }
//
//            tagChunk.put("Cells", nbt::tag_byte_array(std::vector<int8_t>(cells, cells+CELLS_ALL_SIZE)));
//        }
//
//        nbt::io::write_tag("Chunk", tagChunk, file);

    }

    // .vst Volume Store
    [[nodiscard]] std::string _File(glm::vec3 chunkpos) const {
        assert(Chunk::validchunkpos(chunkpos));
        return Strings::fmt("{}/{}.{}.{}.vst", m_ChunkDir, chunkpos.x, chunkpos.y, chunkpos.z);
    }

};

#endif //ETHERTIA_CHUNKLOADER_H
