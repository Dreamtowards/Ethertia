//
// Created by Dreamtowards on 2023/1/15.
//

#pragma once

#include <map>

#include <ethertia/util/Assert.h>
#include <ethertia/util/Collections.h>


template<typename T>
class Registry
{
public:
    // order by string id, for prolonged NumId peroid. for some offline caches (item/material tex atlas)
    std::map<std::string, T*> m_Map;

    /// Runtime NumId. (ordered by string id)
    std::vector<T*> m_Id2Entry;

    T* regist(T* entry) {
        return regist(entry->Id, entry);
    }
    T* regist(const std::string& id, T* entry) {
        assert(m_Id2Entry.empty() && "Registry has been locked since RuntimeIdTable was generated.");
        assert(!has(id) && "Already registered.");
        m_Map[id] = entry;
        return entry;
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
    void _BuildNumIds() 
    {
        m_Id2Entry.clear();
        m_Id2Entry.reserive(size());
        int numId = 0;
        for (auto& it : m_Map) {
            it.NumId = numId;
            m_Id2Entry.push_back(it.second);
            ++numId;
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


    auto begin() {
        return m_Map.begin();
    }
    auto end() {
        return m_Map.end();
    }




    void _DbgPrintEntries(const std::string& name) 
    {
        std::cout << "[" << size() << "] ";
        for (auto& it : m_Map) {
            std::cout << it.first << ", ";
        }
        std::cout << "\n";
    }
};
