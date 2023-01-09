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

class ChunkLoader
{
public:

    class NBT {
    public:
        static nbt::tag_list vec3(glm::vec3 v) {
            return nbt::tag_list({v.x, v.y, v.z});
        }
    };

    std::string m_ChunkDir;

    ChunkLoader(std::string savedir) : m_ChunkDir(std::move(savedir)) {
        assert(!Loader::fileExists(savedir));
    }

    Chunk* loadChunk(glm::vec3 chunkpos, World* world) {return nullptr;
        std::fstream file(_File(chunkpos), std::ios::in);
        if (!file)
            return nullptr;

        Chunk* chunk = new Chunk(chunkpos, world);

        // todo: octrees
        int chunkBlocksSize = sizeof(Cell) * 4096;
        file.read((char*)&chunk->blocks, chunkBlocksSize);




        file.close();
        return chunk;
    }

    void saveChunk(Chunk* chunk) {return;
        std::string filename = _File(chunk->position);
        std::fstream file(filename, std::ios::out);
        Loader::ensureFileParentDirsReady(filename);

        nbt::tag_compound tagChunk;
        tagChunk.put("ChunkPos", NBT::vec3(chunk->position));
        tagChunk.put("CreateTime", 123);
        tagChunk.put("ModifyTime", 345);
        tagChunk.put("InhabitedTime", 567.2f);  // InhabitedTime


        nbt::io::stream_writer nbtWrite(file);
        nbtWrite.write_tag("", tagChunk);

    }

    // .vst Volume Store
    [[nodiscard]] std::string _File(glm::vec3 chunkpos) const {
        assert(Chunk::validchunkpos(chunkpos));
        return Strings::fmt("{}/{}.{}.{}.vst", m_ChunkDir, chunkpos.x, chunkpos.y, chunkpos.z);
    }

};

#endif //ETHERTIA_CHUNKLOADER_H
