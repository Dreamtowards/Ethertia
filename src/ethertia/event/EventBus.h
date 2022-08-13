//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_EVENTBUS_H
#define ETHERTIA_EVENTBUS_H

#include <vector>
#include <unordered_map>
#include <functional>

#include <ethertia/util/UnifiedTypes.h>
#include <ethertia/util/Collections.h>
#include <ethertia/util/Log.h>
#include <cassert>


class EventBus
{
    class EventPriority
    {
    public:
        static constexpr int LOWEST = 3;
        static constexpr int LOW = 4;
        static constexpr int NORMAL = 5;
        static constexpr int HIGH = 6;
        static constexpr int HIGHEST = 7;
        static constexpr int MONITOR = 8;
    };

    class Listener
    {
    public:
        u32 eventId;

        std::function<void(void*)> function;

        int priority = EventPriority::NORMAL;  // higher value, higher priority.
    };

    std::unordered_map<u32, std::vector<Listener>> listeners;

public:
    class InternalUtil
    {
    public:
        template<typename Lambda, typename E>
        static E GetLambdaFuncPtrEventType(void (Lambda::*)(E*));
        template<typename Lambda, typename E>
        static E GetLambdaFuncPtrEventType(void (Lambda::*)(E*) const);

        template<typename Lambda>
        using GetLambdaEventType = decltype(GetLambdaFuncPtrEventType(&Lambda::operator()));

        // (Event) "TypeId". unique for each type.
        template<typename E>
        static u32 GetEventId() {
            static char val = 0;
            return (size_t)&val;
        }

        template<typename DstType, typename SrcType>
        static DstType ultimate_cast(SrcType src) {
            return *reinterpret_cast<DstType*>(&src);
        }
    };

    std::function<bool(const Listener&, const Listener&)> COMP_LSR_PRIORITY = [](const Listener& lhs, const Listener& rhs) {
        return lhs.priority < rhs.priority;
    };

public:

    template<typename E>
    bool post(E* e) const {
        u32 eventId = InternalUtil::GetEventId<E>();

        auto it = listeners.find(eventId);
        if (it == listeners.end())
            return false;

        try {
            for (const auto& lsr : it->second)
            {
                lsr.function(e);
            }
        } catch (ERR_CANCEL) {
            return true;
        }

        return false;
    }


#define FUNC_EL const std::function<void(E*)>&

    template<typename E>
    Listener* listen(FUNC_EL lsrfunc) {
        u32 eventId = InternalUtil::GetEventId<E>();
        Listener& lsr =  listeners[eventId].emplace_back();

        lsr.eventId = eventId;
        lsr.function = InternalUtil::ultimate_cast<std::function<void(void*)>>(lsrfunc);

        return &lsr;
    }

    // static funcptr.
    template<typename E>
    Listener* listen(void (*funcptr)(E*)) {
        return listen<E>((FUNC_EL)funcptr);
    }

    // non-static funcptr
    template<typename E, typename T>
    Listener* listen(void (T::* funcptr)(E*), T* inst) {
        return listen<E>(std::bind(funcptr, inst, std::placeholders::_1));
    }

    // lambda object.
    template<typename Lambda, typename E = InternalUtil::GetLambdaEventType<Lambda>>
    Listener* listen(Lambda l) {
        return listen<E>((FUNC_EL)l);
    }


    void unlisten(Listener* lsr) {
        auto& ls = listeners[lsr->eventId];
        int i = Collections::indexOf(ls, lsr); assert(i != -1);
        ls.erase(ls.begin() + i);
    }

    template<typename E>
    void unlistenAll() {
        u32 eventId = InternalUtil::GetEventId<E>();
        listeners[eventId].clear();
    }

    void unlistenAll() {
        listeners.clear();
    }

    std::unordered_map<u32, std::vector<Listener>>* getListeners() {
        return &listeners;
    }

    void updateListener(Listener* lsr) {
        auto& ls = listeners[lsr->eventId];
        // check exists
        if (Collections::indexOf(ls, lsr) == -1)
            throw std::logic_error("No such listener exists in the eventbus.");

        // update priority.
        std::sort(ls.begin(), ls.end(), COMP_LSR_PRIORITY);
    }



    static EventBus EVENT_BUS;

    static constexpr struct ERR_CANCEL {} FORCE_CANCEL{};
};

// todo
// priority sort
// event cancel. and 'ignoreCancelled' listeners

#endif //ETHERTIA_EVENTBUS_H
