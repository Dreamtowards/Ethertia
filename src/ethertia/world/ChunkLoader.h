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

/*
 * World Storage:
 *
 * - world.dat   # world info.
 * - players/
 *   - 5d2db1fc-4d29-304e-8de8-4879f69fd8d5.dat   # <uuid>.dat
 * - chunks/
 *   - 256.0.512.vst   # chunk bundle.
 */

class ChunkLoader
{
public:
    using vec3 = glm::vec3;

    std::string m_ChunkDir;

    struct VolumeStore {
        struct ChunkDataInfo {
            uint32_t pos;
            uint32_t len;
        };

        std::unordered_map<glm::vec3, ChunkDataInfo> heaptable;  // key: world-coordinate chunkpos.
        std::fstream* file;
    };
    std::unordered_map<glm::vec3, std::fstream*> m_LoadedChunkFiles;

    bool m_DisableSave = false;
    bool m_DisableLoad = false;

    ChunkLoader(const std::string& savedir) : m_ChunkDir(savedir) {

    }



    std::string _FileWorldInfo() const {
        return Strings::fmt("{}/world.dat", m_ChunkDir);
    }

    void loadWorldInfo(World& world) {
        std::ifstream file(_FileWorldInfo(), std::ios::in | std::ios::binary);
        if (!file) return;

        auto tagWorld = nbt::io::read_compound(file).second;

        world.m_Name = (std::string)tagWorld->at("Name");
        world.m_Seed = (int64_t)tagWorld->at("Seed");
        world.m_DayTime = (float)tagWorld->at("DayTime");
        world.m_InhabitedTime = (float)tagWorld->at("InhabitedTime");

        file.close();
    }

    void saveWorldInfo(const World& world) const {
        std::ofstream file(Loader::ensureFileParentDirsReady(_FileWorldInfo()), std::ios::out | std::ios::binary);

        nbt::tag_compound tagWorld;
        tagWorld.put("Name", world.m_Name);
        tagWorld.put("Seed", (int64_t)world.m_Seed);
        tagWorld.put("DayTime", world.m_DayTime);
        tagWorld.put("InhabitedTime", world.m_InhabitedTime);
        tagWorld.put("SavedTime", (int64_t)Timer::timestampMillis());

        nbt::io::write_tag("WorldInfo", tagWorld, file);
        file.close();
    }




/**
 * VST Chunk Store Format.
 *
 * the VST file:
 * +-----------------------+
 * | ChunkData Info Table  | 16*16*16 (4096) entries of {u32 pos, u32 len}, order: x*256 + y*16 + z (why this order?)
 * +-----------------------+
 * | ... ChunkData[]       | ChunkData[] has no arrangement order.
 * +-----------------------+
 *
 * - Chunks 何时保存? 理论上在卸载时保存 即可保存完整(毕竟最后关闭时会卸载所有区块)。
 *   但实际上如果保存出错，或中途闪退/停电 那就会丢失/保存失败。所以也应该定时保存
 * - 哪些Chunks需要保存? 应该是全局保存，而无序标记"needsSave"。因为世界变化太多，生物移动.. 大概率都要保存。而频繁标记会产生消耗。
 * 所以在卸载时保存，为了保险起见 也定时保存，全局保存。
 *
 * 保存时应该批量保存还是单独保存？单独保存当然更好的灵活性。但是性能上 每次单独保存可能要重移后面的上千个区块数据。
 * 而批量保存，可以有优化策略 比如批量按顺序写入新文件 就不需要总是移动后面的数据了。这个还没确定，要找到最佳存储方法。
 *
 * ChunkData[]的排列是否应该和查找表顺序一致? 顺序一致的意义是
 * 1. 如果用顺序排列 那么一旦修改ChunkData[i]，就知道后续需要移动的ChunkData对应的那些需要修改的*查找表*是哪些。
 * 2. 给出一个文件内数据位置, 可以快速根据查找表用二分法而非线性法找出对应是哪个区块. 然而这也没什么用。
 * 然而维护这种顺序是需要额外精力/开销的。所以这里优先**不用顺序一致**。
 *
 * 区块排列/存储方法:
 * - 传统的有序排列方法 每当修改一次 就重新移动后面的数据和查找表。优点是空间紧凑 但要频繁移动
 *   - 优化策略: 批量存储 取得所有区块数据 并按序(?)存储为新文件 就无需移动后面。但批量和获取所有 也是有灵活性代价的。
 * - 堆内存的方法 无序排列 查找表 甚至可能有大空隙。每当修改一次 就失效老内存 查找一块新内存。优点是无序移动数据 无需修改后续查找表，但可能有存储空隙(可以接受)。
 */

    std::fstream* getChunkStoreFile(glm::vec3 chunkpos, bool readonly = true) {
        assert(Chunk::validchunkpos(chunkpos));

        auto it = m_LoadedChunkFiles.find(chunkpos);
        if (it != m_LoadedChunkFiles.end())
            return it->second;

        std::string filename = Strings::fmt("{}/chunks/{}.{}.{}.vst", m_ChunkDir, chunkpos.x, chunkpos.y, chunkpos.z);
        if (readonly && !Loader::fileExists(filename)) {
            return nullptr;  // file not exists.
        }

        Loader::ensureFileParentDirsReady(filename);
        std::fstream* file = new std::fstream(filename, std::ios::binary);

        m_LoadedChunkFiles[chunkpos] = file;
        return file;
    }

    static int ChunkDataInfoAddr(vec3 chunkpos) {
        assert(Chunk::validchunkpos(chunkpos));

        // to local region chunk key. xyz: [0, 16)
        vec3 lc = Mth::floor(chunkpos, 256) / 16.0f;
        assert(glm::fract(lc) == glm::vec3{0});

        int idx = (lc.x*256 + lc.y*16 + lc.z);
        static const int CHUNK_DATA_INFO_LEN = 8;
        return idx * CHUNK_DATA_INFO_LEN;
    }

    std::unique_ptr<nbt::tag_compound> getChunkData(vec3 chunkpos)
    {
        std::fstream* file = getChunkStoreFile(chunkpos);
        if (!file)  // no file exists.
            return nullptr;

        file->seekg(ChunkDataInfoAddr(chunkpos));

        uint32_t chunkdata_begin; *file >> chunkdata_begin;
        uint32_t chunkdata_len;   *file >> chunkdata_len;

        if (chunkdata_begin == 0 || chunkdata_len == 0)  // no chunk stored there.
            return nullptr;

        file->seekg(chunkdata_begin);
        return nbt::io::read_compound(*file).second;
    }

    void setChunkData(vec3 chunkpos, const nbt::tag_compound& chunkdata)
    {
        std::fstream& file = *getChunkStoreFile(chunkpos, false);

        // invalidate old store.


        // allocate an new store.


    }

    Chunk* loadChunk(glm::vec3 chunkpos, World* world) {
        auto tagChunk = getChunkData(chunkpos);
        if (!tagChunk) return nullptr;

        Chunk* chunk = new Chunk(chunkpos, world);

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

        return chunk;
    }

    void saveChunk(Chunk* chunk)
    {
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


        setChunkData(chunk->chunkpos(), tagChunk);
    }

};

#endif //ETHERTIA_CHUNKLOADER_H
