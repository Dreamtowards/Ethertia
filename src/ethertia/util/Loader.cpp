//
// Created by Dreamtowards on 2023/2/1.
//

#include <ethertia/util/Loader.h>
#include <ethertia/util/OBJLoader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>



void Loader::saveOBJ(const std::string &filename, size_t verts, const float *pos, const float *uv, const float *norm)  {
    std::stringstream ss;
    OBJLoader::saveOBJ(ss, verts, pos, uv, norm);

    ensureFileParentDirsReady(filename);
    std::ofstream fs(filename);
    fs << ss.str();
    fs.close();
}

void Loader::loadOBJ_Tiny(const char *filename, VertexBuffer &vbuf) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool succ = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename);
    if (!succ) {
        throw std::runtime_error(Strings::fmt("failed load obj tiny: ", err));
    }

    for (tinyobj::shape_t& shape : shapes)
    {
        tinyobj::mesh_t& mesh = shape.mesh;
        size_t idx_offset = 0;

        vbuf.reserve(vbuf.vertexCount() + mesh.indices.size());

        for (int face_i = 0; face_i < mesh.num_face_vertices.size(); ++face_i)
        {
            int face_verts = mesh.num_face_vertices[face_i];
            for (int fvi = 0; fvi < face_verts; ++fvi)
            {
                tinyobj::index_t idx = mesh.indices[idx_offset + fvi];

                int ip = 3*idx.vertex_index;
                tinyobj::real_t px = attrib.vertices[ip],
                        py = attrib.vertices[ip+1],
                        pz = attrib.vertices[ip+2];
                vbuf.addpos(px,py,pz);

                if (idx.normal_index >= 0) {
                    int in = 3*idx.normal_index;
                    tinyobj::real_t nx = attrib.normals[in],
                            ny = attrib.normals[in+1],
                            nz = attrib.normals[in+2];
                    vbuf.addnorm(nx,ny,nz);
                }

                if (idx.texcoord_index >= 0) {
                    int it = 2*idx.texcoord_index;
                    tinyobj::real_t tx = attrib.texcoords[it],
                            ty = attrib.texcoords[it+1];
                    vbuf.adduv(tx,ty);
                }
            }
            idx_offset += face_verts;

            // mesh.material_ids[face_i];
        }
    }
}


#include <stb/stb_vorbis.c>


int16_t* Loader::loadOGG(std::pair<char*, size_t> data, size_t* dst_len, int* dst_channels, int* dst_sampleRate) {
    int channels = 0;
    int sample_rate = 0;
    int16_t* pcm = nullptr;
    int len = stb_vorbis_decode_memory((unsigned char*)data.first, data.second, &channels, &sample_rate, &pcm);
    if (len == -1)
        throw std::runtime_error("failed decode ogg.");
    assert(pcm);

    // Log::info("Load ogg, pcm size {}, freq {}, cnls {}", Strings::size_str(len), sample_rate, channels);
    assert(channels == 2 || channels == 1);

    *dst_channels = channels;
    *dst_len = len;
    *dst_sampleRate = sample_rate;

    return pcm;
}

