//
// Created by Dreamtowards on 2023/1/15.
//

#ifndef ETHERTIA_REGISTRY_H
#define ETHERTIA_REGISTRY_H

#include <map>

#include <ethertia/util/Collections.h>
#include <ethertia/util/Log.h>

template<typename T>
class Registry
{
public:
    // CNS unordered_map 还是 ordered_map 还是 vector? (现在要string id, 但偶尔也需要数字id 比如离线存储时)
    // 如果要全局数字id的话，那么后两者 中者是'缓存版'数字id (一旦注册表有改动/新物品) 那么之前的数字id失效
    // 最后者是'永久版'数字id 因为是按照顺序注册 顺序是永久的。但这种数字id 如果mod要添加新物品 就可能数字id冲突 或移除后 数字id失效/错乱
    // 所以为了可以自由添加/移除新物品，unordered_map是最佳的。毕竟数字id一般只在离线存储时用，可以用局部查找表。
    // 然而还是打算用map 排序的。毕竟没什么坏处/性能损耗。而且易用于"当前版本缓存"

    // No unordered_map. order is guaranteed to be identical when containing same IDs. (now for mtl-atlas-cache indexing)
    std::map<std::string, T*> m_Map;

//    // 运行时数字id表。全部注册完后 生成。用于一些高性能组件 比如渲染时用数字id表示material类型
//    std::vector<T*> m_RuntimeNumIdTable;

    T* regist(T* t) {
//        assert(m_RuntimeNumIdTable.empty() && "Cache table already built, cannot modify registry.");
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

//    // lock().
//    // 当所有内容注册完后，就锁住. 否则中途修改 运行时数字id表就会错乱 对不上.
//    // e.g. 一些高性能组件需要数字id表，比如渲染时 要用数字id表示material类型
//    void build()
//    {
//        m_RuntimeNumIdTable.reserve(size());
//        for (auto& it : m_Map) {
//            m_RuntimeNumIdTable.push_back(it.second);
//        }
//    }
//
//    int getRuntimeId(T* entry) {
//        assert(!m_RuntimeNumIdTable.empty() && "RuntimeNumIdTable have not build yet.");
//        return Collections::find(m_RuntimeNumIdTable, entry);
//    }

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


    void dbgPrintEntries(const std::string& name) {
        Log::info("Registered {} {}: \1", size(), name);
        for (auto& it : m_Map) {
            std::cout << it.first << ", ";
        }
        std::cout << "\n";
    }
};

#endif //ETHERTIA_REGISTRY_H
