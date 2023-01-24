//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_COLLECTIONS_H
#define ETHERTIA_COLLECTIONS_H

#include <vector>
#include <algorithm>
#include <unordered_map>

class Collections
{
public:
    template<typename T>
    static int find(const std::vector<T>& ls, const T& val) {
        int i = 0;
        for (auto it = ls.begin(); it != ls.end(); ++it) {
            if (*it == val)
                return i;
            i++;
        }
        return -1;
    }

    template<typename T>
    static size_t indexOf(const std::vector<T>& ls, T* p) {
        return ((size_t)p - (size_t)ls.data()) / sizeof(T);
    }

    template<typename T>
    static bool contains(const std::vector<T>& ls, const T& e) {
        return std::find(ls.begin(), ls.end(), e) != ls.end();
    }

    template<typename K, typename V>
    static bool existsKey(const std::unordered_map<K, V>& c, const K& k) {
        return c.find(k) != c.end();
    }

    template<typename T>
    static void erase(std::vector<T>& ls, const T& r) {
        int i = find(ls, r);
        ls.erase(ls.begin() + i);
    }

    template<typename T>
    static void eraseAll(std::vector<T>& ls, const std::vector<T>& tobeErase) {
        for (const T& e : tobeErase) {
            Collections::erase(ls, e);
        }
    }

    template<typename T>
    static T* range(T* arr, int n, T from = 0) {
        for (int i = 0;i < n;++i) {
            arr[i] = from + i;
        }
        return arr;
    }

    template<typename T>
    static void push_back_all(std::vector<T>& dst, std::vector<T>& add) {
        dst.insert(dst.end(), add.begin(), add.end());
    }

    static void cpy(const void* src, void* dst, std::size_t len)
    {
        for (int i = 0; i < len; ++i)
        {
            ((char*)dst)[i] = ((char*)src)[i];
        }
    }

private:
    template<typename Itr, typename T>
    static Itr find_ptr(Itr first, Itr last, T* p) {
        for (auto it = first; it != last; ++it) {
            if (&*it == p)
                return it;
        }
    }
};

#endif //ETHERTIA_COLLECTIONS_H
