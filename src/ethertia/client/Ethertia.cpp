

#include "Ethertia.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <ethertia/event/EventBus.h>

#include <dexode/EventBus.hpp>

void evtHDL(WorldEvent* e) {

    Log::info(">> FuncPtr Handler");
}

#include "dexode/EventBus.hpp"

namespace Namsp {

    class SomeEvt {
    public:

        static void func(WorldEvent* e) {
            Log::info(">> Inside FuncPtr Handler");
        }
    };
    class AnotherEvnt {

    };
}




int main()
{
//    Ethertia().run();

    std::vector<int> ls {1, 2, 3, 4};

    int* p = &ls[2];
//    ls.erase_by_ptr(p);

    Log::info("Idx: ", Collections::indexOf(ls, p));
    ls.erase(ls.begin() + Collections::indexOf(ls, p));
    // seems so stupid: should directly access, instead of iterate..


    Log::info("E0: {}, Beg: {}", p);

    EventBus eventbus;

     std::function<void(WorldEvent*)> hdl = [](WorldEvent* e) {
         Log::info(">> StoredLambda Handler");
     };

    void (*func)(WorldEvent*) = evtHDL;

    eventbus.listen(evtHDL);
    eventbus.listen(func);
    eventbus.listen(Namsp::SomeEvt::func);
    eventbus.listen(hdl);
    eventbus.listen([](WorldEvent* e) {
        Log::info(">> RValLambda Handler");
    });
    eventbus.post(new WorldEvent);

    return 0;
}

