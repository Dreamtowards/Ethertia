//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_EVENTBUS_H
#define ETHERTIA_EVENTBUS_H

#include <vector>
#include <unordered_map>
#include <functional>

#include <ethertia/event/Event.h>
#include <ethertia/event/EventPriority.h>
#include <ethertia/util/Log.h>


class EventBus
{
    class Listener
    {
    public:
        uint eventId;

        std::function<void(void*)> function;

        // int priority = EventPriority::NORMAL;  // higher value, higher priority.
    };

    std::unordered_map<uint, std::vector<Listener>> listeners;

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
        static uint GetEventId() {
            static char val = 0;
            return (size_t)&val;
        }

        template<typename DstType, typename SrcType>
        static DstType ultimate_cast(SrcType src) {
            return *reinterpret_cast<DstType*>(&src);
        }
    };

public:

    template<typename E>
    void post(E* e) const {
        uint eventId = InternalUtil::GetEventId<E>();

        auto it = listeners.find(eventId);
        if (it == listeners.end())
            return;

        for (const auto& lsr : it->second)
        {
            if (lsr.eventId == eventId) {
                lsr.function(e);
            }
        }

    }

#define FUNC_EL const std::function<void(E*)>&

    template<typename E>
    Listener* listen(FUNC_EL lsrfunc) {
        uint eventId = InternalUtil::GetEventId<E>();
        Listener& lsr = listeners[eventId].emplace_back();

        lsr.eventId = eventId;
        lsr.function = InternalUtil::ultimate_cast<std::function<void(void*)>>(lsrfunc);

        return &lsr;
    }

    template<typename E>
    Listener* listen(void (*funcptr)(E*)) {
        return listen<E>((FUNC_EL)funcptr);
    }

    template<typename Lambda, typename E = InternalUtil::GetLambdaEventType<Lambda>>
    Listener* listen(Lambda l) {
        return listen<E>((FUNC_EL)l);
    }

    void unlisten(Listener* lsr) {
        auto& ls = listeners[lsr->eventId];
        ls.erase(ls.begin() + Collections::indexOf(ls, lsr));
    }

    template<typename E>
    void unlistenAll() {
        uint eventId = InternalUtil::GetEventId<E>();
        listeners[eventId].clear();
    }

};

#endif //ETHERTIA_EVENTBUS_H
