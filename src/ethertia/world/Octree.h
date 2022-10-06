////
//// Created by Dreamtowards on 2022/9/11.
////
//
//#ifndef ETHERTIA_OCTREE_H
//#define ETHERTIA_OCTREE_H
//
//#include <ethertia/material/stat/MaterialStat.h>
//
//class Octree
//{
//public:
//    inline static u8 T_LEAF = 1;
//    inline static u8 T_INTERNAL = 2;
//
//    virtual char type() = 0;
//
//    bool isLeaf() { return type() == T_LEAF; }
//    bool isInternal() { return type() == T_INTERNAL; }
//};
//
//
//
//class OctreeLeaf : public Octree
//{
//public:
//    MaterialStat stat;
//
//    char type() override { return Octree::T_LEAF; }
//};
//
//
//
//class OctreeInternal : public Octree
//{
//public:
//    Octree** children;
//
//    char type() override { return Octree::T_INTERNAL; }
//};
//
//#endif //ETHERTIA_OCTREE_H
