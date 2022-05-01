//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_COLLECTIONS_H
#define ETHERTIA_COLLECTIONS_H

#include <vector>
#include <algorithm>

class Collections
{
public:
    template<typename T>
    static int indexOf(const std::vector<T>& ls, const T& el) {
        int i;
        for (auto it = ls.begin(); it != ls.end(); ++it) {
            if (*it == el)
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
