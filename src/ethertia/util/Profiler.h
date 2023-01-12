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
            assert(_n.length() > 0);
            for (Section& s : sections) {
                if (s.name == _n)
                    return s;
            }
            Section& sec = sections.emplace_back();
            sec.name = _n;
            return sec;
        }
        void reset() {
            sumTime = 0;
            numExec = 0;
            _begin = 0;
            _avgTime = 0;
            _lasTime = 0;
            for (Section& s : sections) {
                s.reset();
            }
        }
    };
    Section* sectionToBeClear = nullptr;  // clear section should after it's pop().
    std::thread::id m_LocalThreadId;

    Section m_RootSection;

    Section* m_CurrentSection = &m_RootSection;

    void push(std::string_view name)
    {
#ifndef _NDEBUG
        if (m_RootSection.sections.size() == 0) {
            m_LocalThreadId = std::this_thread::get_id();
        } else {
            assert(m_RootSection.sections.size() == 1);
            assert(m_LocalThreadId == std::this_thread::get_id());
        }
#endif


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
            m_CurrentSection->reset();
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

    Section& GetRootSection() {
        assert(m_RootSection.sections.size() == 1);
        return m_RootSection.sections.at(0);
    }
    void laterClearRootSection() {
        if (m_CurrentSection == &m_RootSection) {
            m_RootSection.reset();
        } else {
            // Delay clear after pop.
            sectionToBeClear = &GetRootSection();
        }
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
