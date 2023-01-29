//
// Created by Dreamtowards on 2022/9/11.
//

#ifndef ETHERTIA_MATERIAL_H
#define ETHERTIA_MATERIAL_H

#include <ethertia/init/Registry.h>
#include <ethertia/item/Item.h>

class Material
{
public:

    inline static Registry<Material> REGISTRY;


    std::string m_Name;


//    // client可能需要一个数字 MtlId, 用于shaders判断vert.MtlId, 用于Atlas缓存mtl_tex索引(离线持久的数字id 直到atlas失效)
//    // 其实前者还好 可以运行时宏替换shaders取得id. 然而后者 让atlas根据其有效周期自己维护一个离线id吧
//    // client. for mtl texture rendering
    int m_TexId = 0;

    bool m_CustomMesh = false;
    void* m_VertexBuffer = nullptr;


    Item* m_MaterialItem = nullptr;


    Material(const std::string& name, bool mesh = false) : m_Name(name), m_CustomMesh(mesh) {

        REGISTRY.regist(this);
    }

    const std::string getRegistryId() const {
        return m_Name;
    }

};

#endif //ETHERTIA_MATERIAL_H
