//
// Created by Dreamtowards on 2022/9/11.
//

#ifndef ETHERTIA_MATERIALS_H
#define ETHERTIA_MATERIALS_H

#include <ethertia/world/Cell.h>

#include <ethertia/material/Material.h>
#include <ethertia/init/Items.h>

class Materials
{
public:

#define REGISTER_MTL(VAR, id) inline static Material* VAR = new Material{id};

    // 用0/nullptr 表示空气/空. 因为实在是太方便了 if(c.mtl)。而且目前没什么副作用
    inline static Material* AIR = nullptr;

    REGISTER_MTL(GRASS, "grass");
    REGISTER_MTL(MOSS, "moss");
    REGISTER_MTL(DIRT, "black_dirt");
    REGISTER_MTL(DIRT_, "dirt");
    REGISTER_MTL(SAND, "sand");

    REGISTER_MTL(LEAVES, "leaves");
    REGISTER_MTL(TALLGRASS, "tallgrass");
    REGISTER_MTL(WATER, "water");
    REGISTER_MTL(LOG, "oak_log");
    REGISTER_MTL(PLANKS, "plank");
    REGISTER_MTL(FARMLAND, "farmland");

    REGISTER_MTL(STONE, "stone");
    REGISTER_MTL(TUFF, "tuff");
    REGISTER_MTL(CONCRETE, "concrete");
    REGISTER_MTL(ROCK, "rock");
    REGISTER_MTL(CLIFF_ROCK, "cliff_rock");
    REGISTER_MTL(VOLCANIC, "volcanic_rock");
    REGISTER_MTL(JUNGLE_ROCK, "jungle_rock");
    REGISTER_MTL(DEEPSLATE, "deepslate");



    REGISTER_MTL(STONE_BRICK, "stone_brick");
    REGISTER_MTL(BRICK_ROOF, "brick_roof");
    REGISTER_MTL(MEADOW, "meadow");

    REGISTER_MTL(IRON, "iron");
    REGISTER_MTL(IRON_TREADPLATE, "iron_treadplate");



    REGISTER_MTL(STOOL, "stool");







    // CNS 关于 MaterialItems, 应该每个material对应一个新物品吗? 还是所有的 material 都是同一个 Item::MATERIAL + 额外 MtlId 数据?
    // 如果是前者, 那么可自定义性很大 一个mtl又可以摆放 又可以吃 又可以当工具.. 然而这种情况不常见?
    // 如果是后者 那么不会有什么'id污染' 和特例。比如现在加载mtl item的材质时 要特例去读materials的资源 但其实就算后者也要读。
    // 所以目前用前者 毕竟mtl是核心内容。而且还没发现坏处。
//    inline static const Item* STONE = new Item("stone", {new Item::ComponentMaterial(1)});

    static void registerMaterialItems() {
        for (auto& it : Material::REGISTRY) {

            it.second->m_MaterialItem = new Item(it.first, {new ItemComponentMaterial(it.second)});
        }
    }


};

#endif //ETHERTIA_MATERIALS_H
