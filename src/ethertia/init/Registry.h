//
// Created by Dreamtowards on 2023/1/15.
//

#ifndef ETHERTIA_REGISTRY_H
#define ETHERTIA_REGISTRY_H

#include <unordered_map>

template<typename T>
class Registry
{
public:
    std::unordered_map<std::string, T*> m_Map;

    T* regist(T* t) {
        std::string id = t->getRegistryId();

        assert(!has(id) && "Already registered.");
        m_Map[id] = t;
        return t;
    }

    bool has(const std::string& id) const {
        return m_Map.find(id) != m_Map.end();
    }

    const T* get(const std::string& id) {
        assert(has(id));
        return m_Map[id];
    }

    size_t size() const {
        return m_Map.size();
    }

    auto begin() {
        return m_Map.begin();
    }
    auto end() {
        return m_Map.end();
    }

    class Registrable
    {
    public:

        virtual std::string getRegistryId() = 0;

    };


};

#endif //ETHERTIA_REGISTRY_H
