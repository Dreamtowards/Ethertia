//
// Created by Dreamtowards on 2022/9/11.
//

#pragma once

#include <ethertia/util/Registry.h>
#include <ethertia/item/Item.h>
#include <ethertia/item/ItemStack.h>

#include <ethertia/render/VertexData.h>


class Material
{
public:
    inline static Registry<Material> REGISTRY;


    std::string Id;

    /// Hardness
    /// Dirt: 0.8, Stone: 8, Ore: 16, Furnace: 18, Anvil: 25, Obsidian: 250
    float Hardness = 1;  // seconds to dig via hand.

//    // client可能需要一个数字 MtlId, 用于shaders判断vert.MtlId, 用于Atlas缓存mtl_tex索引(离线持久的数字id 直到atlas失效)
//    // 其实前者还好 可以运行时宏替换shaders取得id. 然而后者 让atlas根据其有效周期自己维护一个离线id吧
//    // client. for mtl texture rendering
    int m_TexId = 0;

    VertexData* m_CustomMesh = nullptr;

    /// Vegetable Materials will generated to another mesh., with Double-Sided (NoCulling), NoCollision, WavingVertex Rendering
    bool IsVegetable = false;

    // generate mesh on top of bottom-cell surface. e.g. Crops, Tallgrass.
    //bool m_IsTouchdown = false;

    // Item of the Material
    Item* item = nullptr;

    struct MtlParams {
        float hardness = 1;
        VertexData* mesh = nullptr;
        bool vege = false;
        bool touchdown = false;  // on top of the bottom cell.  align bottom. alignb
    };

    Material(
        const std::string& id,
        const MtlParams& params = {1.0f, nullptr, false, false}) : Id(id) 
    {

        REGISTRY.regist(this);

        m_CustomMesh = params.mesh;
        IsVegetable = params.vege;
        Hardness = params.hardness;

        std::cout << "Mtl " << id << "\n";
    }

};



class Materials
{
public:

#define ET_DECL_MTL(x, ...) inline static Material* x = new Material{__VA_ARGS__};

    /// use 0/nullptr to represents AIR. since it's convinent and no side effect yet.
    inline static Material* AIR = nullptr;

    // Smooth Terrain Materials.

    ET_DECL_MTL(GRASS,         "grass",        { .hardness = 0.8 });
    ET_DECL_MTL(MOSS,          "moss",         { .hardness = 0.8 });
    ET_DECL_MTL(DIRT,          "black_dirt",   { .hardness = 0.8 });
    ET_DECL_MTL(DIRT_,         "dirt",         { .hardness = 0.8 });
    ET_DECL_MTL(SAND,          "sand",         { .hardness = 0.8 });
    ET_DECL_MTL(FARMLAND,      "farmland",     { .hardness = 0.8 });
    ET_DECL_MTL(MEADOW,        "meadow",       { .hardness = 0.8 });

    ET_DECL_MTL(LOG,           "oak_log",      { .hardness = 2.2 });
    ET_DECL_MTL(PLANKS,        "plank",        { .hardness = 1.4 });

    ET_DECL_MTL(STONE,         "stone",        { .hardness = 8.0 });
    ET_DECL_MTL(TUFF,          "tuff",         { .hardness = 8.0 });
    ET_DECL_MTL(CONCRETE,      "concrete",     { .hardness = 8.0 });
    ET_DECL_MTL(ROCK,          "rock",         { .hardness = 8.0 });
    ET_DECL_MTL(ROCK_MOSSY,    "rock_mossy",   { .hardness = 8.0 });
    ET_DECL_MTL(ROCK_SMOOTH,   "rock_smooth",  { .hardness = 8.0 });
    ET_DECL_MTL(CLIFF_ROCK,    "cliff_rock",   { .hardness = 8.0 });
    ET_DECL_MTL(VOLCANIC_ROCK, "volcanic_rock",{ .hardness = 8.0 });
    ET_DECL_MTL(JUNGLE_ROCK,   "jungle_rock",  { .hardness = 8.0 });
    ET_DECL_MTL(DEEPSLATE,     "deepslate",    { .hardness = 14.0 });
    ET_DECL_MTL(STONE_BRICK,   "stone_brick",  { .hardness = 8.0 });
    ET_DECL_MTL(BRICK_ROOF,    "brick_roof",   { .hardness = 8.0 });

    ET_DECL_MTL(IRON,          "iron",         { .hardness = 28.0 });
    ET_DECL_MTL(IRON_TREADPLATE, "iron_treadplate", { .hardness = 28.0 });



    
    // Vegetable/Foliage
    ET_DECL_MTL(WATER,         "water",        { .hardness = INFINITY, .vege = true });
    //ET_DECL_MTL(LEAVES,        "leaves",       { .hardness = 0, .mesh = true, .vege = true });
    //ET_DECL_MTL(SPRUCE_LEAVES, "spruce_leaves",{ .hardness = 0, .mesh = true, .vege = true });
    //ET_DECL_MTL(TALL_GRASS,    "tall_grass",   { .hardness = 0, .mesh = true, .vege = true, .touchdown=true });
    //ET_DECL_MTL(SHORT_GRASS,   "short_grass",  { .hardness = 0, .mesh = true, .vege = true, .touchdown=true });
    //ET_DECL_MTL(TALL_FERN,     "tall_fern",    { .hardness = 0, .mesh = true, .vege = true, .touchdown=true });
    //ET_DECL_MTL(SHRUB,         "shrub",        { .hardness = 0, .mesh = true, .vege = true, .touchdown=true });
    //ET_DECL_MTL(ROSE_BUSH,     "rose_bush",    { .hardness = 0, .mesh = true, .vege = true, .touchdown=true });
    //
    //// Crops
    //ET_DECL_MTL(CARROTS,       "carrots",      { .hardness = 0, .mesh=true, .vege=true, .touchdown=true });
    //ET_DECL_MTL(POTATOES,      "potatoes",     { .hardness = 0, .mesh=true, .vege=true, .touchdown=true });
    //
    //// Decorations.
    //ET_DECL_MTL(STOOL,         "stool",        { .hardness = 1.4, .mesh = true });
    //ET_DECL_MTL(SHORT_STOOL,   "short_stool",  { .hardness = 1.4, .mesh = true });
    //ET_DECL_MTL(CHAIR,         "chair",        { .hardness = 1.4, .mesh = true });
    //ET_DECL_MTL(CASH_REGISTER, "cash_register",{ .hardness = 1.4, .mesh = true });
    //ET_DECL_MTL(LADDER,        "ladder",       { .hardness = 1.4, .mesh = true });



    // CNS 关于 MaterialItems, 应该每个material对应一个新物品吗? 还是所有的 material 都是同一个 Item::MATERIAL + 额外 MtlId 数据?
    // 如果是前者, 那么可自定义性很大 一个mtl又可以摆放 又可以吃 又可以当工具.. 然而这种情况不常见?
    // 如果是后者 那么不会有什么'id污染' 和特例。比如现在加载mtl item的材质时 要特例去读materials的资源 但其实就算后者也要读。
    // 所以目前用前者 毕竟mtl是核心内容。而且还没发现坏处。
    //static void _InitMtlItems()
    //{
    //    for (auto& it : Material::REGISTRY)
    //    {
    //        it.second->m_MaterialItem = new Item(it.first, {new ItemComponentMaterial(it.second)});
    //    }
    //}


};