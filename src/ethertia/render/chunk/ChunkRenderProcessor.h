//
// Created by Dreamtowards on 2022/12/10.
//

#ifndef ETHERTIA_CHUNKRENDERPROCESSOR_H
#define ETHERTIA_CHUNKRENDERPROCESSOR_H

class ChunkRenderProcessor
{
public:
    using vec3 = glm::vec3;


    static void meshChunk_Upload(Chunk* chunk) {
        chunk->needUpdateModel = false;

        VertexBuffer* vbuf = nullptr;

        {
//        BenchmarkTimer _tm;
//        Log::info("Chunk MeshGen. \1");

//        vbuf = BlockyChunkMeshGen::genMesh(chunk, world);
            vbuf = SurfaceNetsMeshGen::contouring(chunk);
//        vbuf = MarchingCubesMeshGen::genMesh(chunk);
//        vbuf = BlockyMeshGen::gen(chunk, new VertexBuffer);
        }


//#define ETHERIA_EXT_FixNormAvgAtChunkBoundary
#ifndef ETHERIA_EXT_FixNormAvgAtChunkBoundary
        {
            vbuf->normals.reserve(vbuf->vertexCount() * 3);
            VertexProcess::gen_avgnorm(vbuf->vertexCount(), vbuf->positions.data(), vbuf->vertexCount(), vbuf->normals.data());
        }
#else
        // fix of Normal Smoothing at Chunk Boundary
    {
        using glm::vec3;
        BenchmarkTimer _tm;

        vbuf->normals.reserve(vbuf->vertexCount() * 3);

        const vec3 faces[6] = {
                vec3(-1, 0, 0), vec3(1, 0, 0),
                vec3(0, -1, 0), vec3(0, 1, 0),
                vec3(0, 0, -1), vec3(0, 0, 1)
        };

        chunk->vert_positions.clear();
        Collections::push_back_all(chunk->vert_positions, vbuf->positions);

        vec3 cp = chunk->position;

        std::vector<float> all_pos;
        Collections::push_back_all(all_pos, chunk->vert_positions);

        int _numSamp = 0;
        for (int face_i = 0; face_i < 6; ++face_i) {
            const vec3& dif = faces[face_i] * 16.0f;

            if (Chunk* bc = chunk->world->getLoadedChunk(cp + dif)) { _numSamp++;
                std::vector<float>& bp = bc->vert_positions;

                int tris = bp.size() / 9;
                for (int tri_i = 0; tri_i < tris; ++tri_i) {
                    // todo: Optimize, Only add Near Boundary Face's Triangle Vertices.
//                    vec3 _p = Mth::vec3(&bp[tri_i*9]);
//                    if (_p.x ) {
//                        continue;
//                    }

                    all_pos.push_back(bp[tri_i*9]   + dif.x);
                    all_pos.push_back(bp[tri_i*9+1] + dif.y);
                    all_pos.push_back(bp[tri_i*9+2] + dif.z);

                    all_pos.push_back(bp[tri_i*9+3]  + dif.x);
                    all_pos.push_back(bp[tri_i*9+4] + dif.y);
                    all_pos.push_back(bp[tri_i*9+5] + dif.z);

                    all_pos.push_back(bp[tri_i*9+6] + dif.x);
                    all_pos.push_back(bp[tri_i*9+7] + dif.y);
                    all_pos.push_back(bp[tri_i*9+8] + dif.z);
                }
            }
        }
        Log::info("NormAvg. Neib {}, {}/{} \1", _numSamp, vbuf->vertexCount(), all_pos.size() / 3);


        // Do the AvgNorm

        VertexProcess::gen_avgnorm(all_pos.size() / 3, all_pos.data(), vbuf->vertexCount(), vbuf->normals.data());
    }
#endif



        if (vbuf) {
            Ethertia::getScheduler()->exec([chunk, vbuf]() {
                delete chunk->proxy->model;

                chunk->proxy->model = Loader::loadModel(vbuf, true);
                chunk->proxy->setMesh(chunk->proxy->model, vbuf->positions.data());

                delete vbuf;
            });
        }
    }



    static Chunk* questNearestInvalidChunk(World* world, vec3 center, int viewDistance) {
        vec3 base_chunkpos = Chunk::chunkpos(center);

        Chunk* nearest = nullptr;
        float  nearest_distSq = FLT_MAX;
        vec3   nearest_pos_gen = vec3(Mth::Inf);

        int n = viewDistance;
        for (int dx = -n;dx <= n;dx++) {
            for (int dz = -n; dz <= n; dz++) {
                for (int dy = -n; dy <= n; dy++) {
                    vec3 dif = vec3(dx, dy, dz) * 16.0f;
                    Chunk* c = world->getLoadedChunk(base_chunkpos + dif);

                    float distSq = glm::length2(dif);
                    if (distSq < nearest_distSq && (!c || c->needUpdateModel)) {
                        nearest = c;
                        nearest_pos_gen = base_chunkpos + dif;
                        nearest_distSq = distSq;
                    }
                }
            }
        }

        if (!nearest && nearest_pos_gen.x != Mth::Inf) {
            // Gen
            nearest = world->provideChunk(nearest_pos_gen);
        }

        return nearest;
    }

    static int unloadChunks_OutOfViewDistance(World* world, vec3 center, int viewDistance) {
        vec3 cpos = Chunk::chunkpos(center);
        int lim = viewDistance * 16;

        std::vector<glm::vec3> unloads;  // separate remove-op from iterate
        for (auto it : world->getLoadedChunks()) {
            vec3 p = it.first;
            if (abs(p.x-cpos.x) > lim ||
                abs(p.y-cpos.y) > lim ||
                abs(p.z-cpos.z) > lim) {
                unloads.push_back(p);
            }
        }

        for (glm::vec3 cp : unloads) {
            world->unloadChunk(cp);
        }
        return unloads.size();
    }


    static void initThreadChunkLoad() {
        new std::thread([]() {
            while (Ethertia::isRunning()) {
                World* world = Ethertia::getWorld();
                if (world)
                {
                    std::lock_guard<std::mutex> guard(world->lock_ChunkList);

                    vec3 p = Ethertia::getCamera()->position;
                    int  n = Ethertia::getRenderEngine()->viewDistance;

                    Chunk* chunk = questNearestInvalidChunk(world, p, n);

                    if (chunk)
                    {
                        Log::info("Mesh Chunk, Upload");
                        meshChunk_Upload(chunk);
                    }

                    int numUnloaded = unloadChunks_OutOfViewDistance(world, p, n);
                    if (numUnloaded) { Log::info("Unloaded {} Chunks", numUnloaded); }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds (1));
            }
        });
    }



//
//    static void checkChunksModelUpdate(World* world) {
//        glm::vec3 vpos = Ethertia::getCamera()->position;
//
//        int i = 0;
//        while (++i <= 4) {
//            Chunk* chunk = nullptr;
//            float minLen = Mth::Inf;
//            for (auto& it : world->getLoadedChunks()) {
//                Chunk* c = it.second;
//                if (c && c->needUpdateModel) {
//                    float len = glm::length(c->position - vpos);
//                    if (len < minLen) {
//                        minLen = len;
//                        chunk = c;
//                    }
//                }
//            }
//            if (chunk == nullptr)  // nothing to update
//                return;
//            chunk->needUpdateModel = false;
//
//            doChunkModelUpload(chunk);
//
//        }
//    }
//    static void makeNearestChunk(World* world, glm::vec3 p, int n)
//    {
//        glm::vec3 cpos = Chunk::chunkpos(p);
//
//        int num = 0;
//        // load chunks
//        for (int dx = -n;dx <= n;dx++) {
//            for (int dz = -n;dz <= n;dz++) {
//                for (int dy = -n;dy <= n;dy++) {
//                    glm::vec3 p = cpos + glm::vec3(dx, dy, dz) * 16.0f;
//                    Chunk* c = world->getLoadedChunk(p);
//                    if (!c) {
//                        world->provideChunk(p);
//                        for (int i = 0; i < 6; ++i) {
//                            world->requestRemodel(p + Mth::QFACES[i] * 16.0f);
//                        }
//                        if (++num > 10)
//                            goto skip;
//                    }
//                }
//            }
//        }
//    }


};

#endif //ETHERTIA_CHUNKRENDERPROCESSOR_H
