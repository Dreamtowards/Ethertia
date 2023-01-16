//
// Created by Dreamtowards on 2022/9/11.
//

#ifndef ETHERTIA_MATERIAL_H
#define ETHERTIA_MATERIAL_H

#include <ethertia/init/Registry.h>

class Material
{
public:

    inline static Registry<Material> REGISTRY;


    std::string m_Name;

    // client. for mtl texture rendering
    int m_AtlasTexIdx = 0;

    Material(const std::string& name) : m_Name(name) {

        REGISTRY.regist(this);
    }

    std::string getRegistryId() const {
        return m_Name;
    }

};

#endif //ETHERTIA_MATERIAL_H
