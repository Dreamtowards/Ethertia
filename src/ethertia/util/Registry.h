//
// Created by Dreamtowards on 2023/1/15.
//

#ifndef ETHERTIA_REGISTRY_H
#define ETHERTIA_REGISTRY_H

#include <map>
#include <cassert>

#include <ethertia/util/Collections.h>
#include <ethertia/util/Log.h>

#define DECL_RegistryId(x) [[nodiscard]] const std::string getRegistryId() const { return x; }

template<typename T>
class Registry
{
public:
    // No unordered_map. order is guaranteed for some offline caches (item/material tex atlas)
    std::map<std::string, T*> m_Map;

    // NumId used in e.g. mtl-id check in shaders, mtl-tex-id atlas indexing. (id value valid as long as String Id List doesn't change)
    std::vector<T*> m_RuntimeNumIdTable;

    T* regist(T* t) {
        std::string id = t->getRegistryId();
        return regist(id, t);
    }
    T* regist(const std::string& id, T* t) {
        assert(m_RuntimeNumIdTable.empty() && "Registry has been locked since RuntimeIdTable was generated.");
        assert(!has(id) && "Already registered.");
        m_Map[id] = t;
        return t;
    }

    [[nodiscard]]
    bool has(const std::string& id) const {
        return get(id) != nullptr;
    }

    // nullable
    const T* get(const std::string& id) const {
        auto it = m_Map.find(id);
        if (it == m_Map.end())
            return nullptr;
        return it->second;
    }

    [[nodiscard]]
    size_t size() const {
        return m_Map.size();
    }

    // NumId for id check in e.g. Atlas-Indexing, Shaders
    void buildRuntimeNumIdTable() {
        int i = 0;
        for (auto& it : m_Map) {
            it.RuntimeId = i;
            m_RuntimeNumIdTable = i;
            ++i;
        }
    }



// todo O(1).
    int getOrderId(const T* entry) {
        // Extreme Low Effective
        int i = 0;
        for (auto& it : m_Map) {
            if (it.second == entry) {
                return i;
            }
            i++;
        }
        return -1;
    }
//    T* getOrderEntry(int n) {
//        int i = 0;
//        for (auto& it : m_Map) {
//            if (i == n) {
//                return it.second;
//            }
//            i++;
//        }
//        return nullptr;
//    }

    auto begin() {
        return m_Map.begin();
    }
    auto end() {
        return m_Map.end();
    }

//    class Registrable
//    {
//    public:
//
//        virtual std::string getRegistryId() = 0;
//
//    };


    void dbgPrintEntries(const std::string& name) {
        Log::info("Registered {} {}: \1", size(), name);
        for (auto& it : m_Map) {
            std::cout << it.first << ", ";
        }
        std::cout << "\n";
    }
};

#endif //ETHERTIA_REGISTRY_H
