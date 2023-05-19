//
// Created by Dreamtowards on 2023/5/19.
//

#ifndef ETHERTIA_RENDERERGBUFFERS_H
#define ETHERTIA_RENDERERGBUFFERS_H



namespace RendererGbuffer
{

    inline struct UBO_Frag_T
    {
        uint32_t MtlTexCap;
        float MtlTexScale = 3.5;
        float MtlTriplanarBlendPow = 6.0f;
        float MtlHeightmapBlendPow = 0.6f;
    } g_uboFrag{};

}



#endif //ETHERTIA_RENDERERGBUFFERS_H
