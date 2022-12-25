//
// Created by Dreamtowards on 2022/12/22.
//

#ifndef ETHERTIA_PROFILER_H
#define ETHERTIA_PROFILER_H

#include <chrono>
#include <vector>
#include <string>
#include <functional>

#include <ethertia/util/Timer.h>
#include "Log.h"

class Profiler
{
public:
    struct Section
    {
        std::string name;
        double sumTime = 0.0f;
        size_t numExec = 0;
        std::vector<Section> sections;
        Section* parent = nullptr;

        // cache
        double _begin = 0.0f;
        double _avgTime = 0.0f;
        double _lasTime = 0.0f;

        Section& find(std::string_view _n) {
            for (Section& s : sections) {
                if (s.name == _n)
                    return s;
            }
            Section& sec = sections.emplace_back();
            sec.name = _n;
            return sec;
        }
    };
    Section* sectionToBeClear = nullptr;  // clear section should after it's pop().

    Section m_RootSection;

    Section* m_CurrentSection = &m_RootSection;

    void push(std::string_view name)
    {
        Section& sec = m_CurrentSection->find(name);

        sec.parent = m_CurrentSection;  // if (sec.parent == nullptr)
        sec._begin = Timer::nanoseconds();

        m_CurrentSection = &sec;
    }

    void pop()
    {
        if (sectionToBeClear == m_CurrentSection) {
            sectionToBeClear = nullptr;
            auto parent = m_CurrentSection->parent;
            *m_CurrentSection = {};
            m_CurrentSection = parent;
            return;
        }

        Section& sec = *m_CurrentSection;

        sec.numExec++;
        double d = (Timer::nanoseconds() - sec._begin) / Timer::NANOSECOND_UNIT;
        sec.sumTime += d;
        sec._avgTime = sec.sumTime / sec.numExec;
        sec._lasTime = d;

        m_CurrentSection = m_CurrentSection->parent;

    }

    class DtorIvkr
    {
    public:
        std::function<void()> fn;

        ~DtorIvkr() {
            fn();
        }
    };
    DtorIvkr push_ap(std::string_view name) {
        push(name);

        return DtorIvkr{[this](){
            pop();
        }};
    }

};

#endif //ETHERTIA_PROFILER_H
