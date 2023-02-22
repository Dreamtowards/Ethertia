//
// Created by Dreamtowards on 2022/9/11.
//

#ifndef ETHERTIA_MATERIALS_H
#define ETHERTIA_MATERIALS_H

#include <ethertia/world/Cell.h>

#include <ethertia/material/Material.h>
#include <ethertia/item/Items.h>

class Materials
{
public:

#define REGISTER_MTL(VAR, ...) inline static Material* VAR = new Material{__VA_ARGS__};

    // 用0/nullptr 表示空气/空. 因为实在是太方便了 if(c.mtl)。而且目前没什么副作用
    inline static Material* AIR = nullptr;



    // Smooth Terrain Materials.

    REGISTER_MTL(GRASS,         "grass",        { .hardness = 0.8 });
    REGISTER_MTL(MOSS,          "moss",         { .hardness = 0.8 });
    REGISTER_MTL(DIRT,          "black_dirt",   { .hardness = 0.8 });
    REGISTER_MTL(DIRT_,         "dirt",         { .hardness = 0.8 });
    REGISTER_MTL(SAND,          "sand",         { .hardness = 0.8 });
    REGISTER_MTL(FARMLAND,      "farmland",     { .hardness = 0.8 });
    REGISTER_MTL(MEADOW,        "meadow",       { .hardness = 0.8 });

    REGISTER_MTL(LOG,           "oak_log",      { .hardness = 2.2 });
    REGISTER_MTL(PLANKS,        "plank",        { .hardness = 1.4 });

    REGISTER_MTL(STONE,         "stone",        { .hardness = 8.0 });
    REGISTER_MTL(TUFF,          "tuff",         { .hardness = 8.0 });
    REGISTER_MTL(CONCRETE,      "concrete",     { .hardness = 8.0 });
    REGISTER_MTL(ROCK,          "rock",         { .hardness = 8.0 });
    REGISTER_MTL(ROCK_MOSSY,    "rock_mossy",   { .hardness = 8.0 });
    REGISTER_MTL(ROCK_SMOOTH,   "rock_smooth",  { .hardness = 8.0 });
    REGISTER_MTL(CLIFF_ROCK,    "cliff_rock",   { .hardness = 8.0 });
    REGISTER_MTL(VOLCANIC_ROCK, "volcanic_rock",{ .hardness = 8.0 });
    REGISTER_MTL(JUNGLE_ROCK,   "jungle_rock",  { .hardness = 8.0 });
    REGISTER_MTL(DEEPSLATE,     "deepslate",    { .hardness = 14.0 });
    REGISTER_MTL(STONE_BRICK,   "stone_brick",  { .hardness = 8.0 });
    REGISTER_MTL(BRICK_ROOF,    "brick_roof",   { .hardness = 8.0 });

    REGISTER_MTL(IRON,          "iron",         { .hardness = 28.0 });
    REGISTER_MTL(IRON_TREADPLATE, "iron_treadplate", { .hardness = 28.0 });




    // Vegetable/Foliage
    REGISTER_MTL(WATER,         "water",        { .hardness = Mth::Inf, .vege = true });
    REGISTER_MTL(LEAVES,        "leaves",       { .hardness = 0, .mesh = true, .vege = true });
    REGISTER_MTL(SPRUCE_LEAVES, "spruce_leaves",{ .hardness = 0, .mesh = true, .vege = true });
    REGISTER_MTL(TALL_GRASS,    "tall_grass",   { .hardness = 0, .mesh = true, .vege = true, .touchdown=true });
    REGISTER_MTL(SHORT_GRASS,   "short_grass",  { .hardness = 0, .mesh = true, .vege = true, .touchdown=true });
    REGISTER_MTL(TALL_FERN,     "tall_fern",    { .hardness = 0, .mesh = true, .vege = true, .touchdown=true });
    REGISTER_MTL(SHRUB,         "shrub",        { .hardness = 0, .mesh = true, .vege = true, .touchdown=true });
    REGISTER_MTL(ROSE_BUSH,     "rose_bush",    { .hardness = 0, .mesh = true, .vege = true, .touchdown=true });

    // Crops
    REGISTER_MTL(CARROTS,       "carrots",      { .hardness = 0, .mesh=true, .vege=true, .touchdown=true });
    REGISTER_MTL(POTATOES,      "potatoes",     { .hardness = 0, .mesh=true, .vege=true, .touchdown=true });

    // Decorations.
    REGISTER_MTL(STOOL,         "stool",        { .hardness = 1.4, .mesh = true });
    REGISTER_MTL(SHORT_STOOL,   "short_stool",  { .hardness = 1.4, .mesh = true });
    REGISTER_MTL(CHAIR,         "chair",        { .hardness = 1.4, .mesh = true });
    REGISTER_MTL(CASH_REGISTER, "cash_register",{ .hardness = 1.4, .mesh = true });
    REGISTER_MTL(LADDER,        "ladder",       { .hardness = 1.4, .mesh = true });







    // CNS 关于 MaterialItems, 应该每个material对应一个新物品吗? 还是所有的 material 都是同一个 Item::MATERIAL + 额外 MtlId 数据?
    // 如果是前者, 那么可自定义性很大 一个mtl又可以摆放 又可以吃 又可以当工具.. 然而这种情况不常见?
    // 如果是后者 那么不会有什么'id污染' 和特例。比如现在加载mtl item的材质时 要特例去读materials的资源 但其实就算后者也要读。
    // 所以目前用前者 毕竟mtl是核心内容。而且还没发现坏处。
//    inline static const Item* STONE = new Item("stone", {new Item::ComponentMaterial(1)});

    static void registerMaterialItems()
    {
        for (auto& it : Material::REGISTRY)
        {
            it.second->m_MaterialItem = new Item(it.first, {new ItemComponentMaterial(it.second)});
        }
    }


};

#endif //ETHERTIA_MATERIALS_H
