//
// Created by Dreamtowards on 2022/9/11.
//

#ifndef ETHERTIA_MATERIAL_H
#define ETHERTIA_MATERIAL_H

#include <ethertia/util/Registry.h>
#include <ethertia/item/Item.h>
#include <ethertia/item/ItemStack.h>

class Material
{
public:
    inline static Registry<Material> REGISTRY;


    std::string m_Name;

    // Dirt: 0.8, Stone: 8, Ore: 16, Furnace: 18, Anvil: 25, Obsidian: 250
    float m_Hardness = 1;  // seconds to dig via hand.

//    // client可能需要一个数字 MtlId, 用于shaders判断vert.MtlId, 用于Atlas缓存mtl_tex索引(离线持久的数字id 直到atlas失效)
//    // 其实前者还好 可以运行时宏替换shaders取得id. 然而后者 让atlas根据其有效周期自己维护一个离线id吧
//    // client. for mtl texture rendering
    int m_TexId = 0;

    bool m_CustomMesh = false;
    void* m_VertexBuffer = nullptr;

    // vegetable another chunk mesh. NoCollision, Render WavingVertex, Render NoFaceCulling.
    bool m_IsVegetable = false;

    // generate mesh on top of bottom-cell surface. e.g. Crops, Tallgrass.
    bool m_IsTouchdown = false;


    Item* m_MaterialItem = nullptr;

    struct MtlParams {
        float hardness = 1;
        bool mesh = false;
        bool vege = false;
        bool touchdown = false;  // on top of the bottom cell.  align bottom. alignb
    };

    Material(const std::string& name,
             const MtlParams& params = {1.0f, false, false, false}
             ) : m_Name(name) {

        REGISTRY.regist(this);

        m_CustomMesh = params.mesh;
        m_IsVegetable = params.vege;
        m_IsTouchdown = params.touchdown;
        m_Hardness = params.hardness;
    }

    DECL_RegistryId(m_Name);

    void getDrops(std::vector<ItemStack>& drops)
    {
        drops.push_back(ItemStack(m_MaterialItem, 1));
    }
};

#endif //ETHERTIA_MATERIAL_H
