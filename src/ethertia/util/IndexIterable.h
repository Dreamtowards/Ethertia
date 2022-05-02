//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_INDEXITERABLE_H
#define ETHERTIA_INDEXITERABLE_H

#include <functional>

template<typename T>
class IndexIterable
{
    std::function<int()> size;
    std::function<T(int)> getter;
public:
    IndexIterable(const std::function<int()>& size, const std::function<T(int)>& getter) : size(size), getter(getter) {}

    class Iter
    {
    public:
        std::function<T(int)> getter;
        int i;
        Iter(const std::function<T(int)>& getter, int i) : getter(getter), i(i) {}

        T operator*() const {
            return getter(i);
        }
        Iter operator++() {
            ++i;
            return *this;
        }
        friend bool operator!=(const Iter& lhs, const Iter& rhs) {
            return lhs.i!=rhs.i;  // really?
        }
    };

public:
    Iter begin() {
        return Iter(getter, 0);
    }
    Iter end() {
        return Iter(getter, size());
    }
};

#endif //ETHERTIA_INDEXITERABLE_H
