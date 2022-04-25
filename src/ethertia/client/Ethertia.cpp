
#include <ethertia/util/Log.h>

#include "Ethertia.h"

Ethertia* Ethertia::INST;

#include <ethertia/world/gen/JRand.h>

int main()
{
//    Ethertia().run();

    JRand jr;
    jr.seed = JRand::initSeed(1772835215);
    Log::info(std::to_string(jr.nextInt()));
    Log::info(std::to_string(jr.nextInt()));
    Log::info(std::to_string(jr.nextDouble()));
    Log::info(std::to_string(jr.nextDouble()));
    Log::info(std::to_string(jr.nextLong()));
    Log::info(std::to_string(jr.nextLong()));
    Log::info(std::to_string(jr.nextInt(100)));
    Log::info(std::to_string(jr.nextInt(10000)));

    Log::info(std::to_string(JRand::hash_jstr("Glacier")));  // 1772835215
    return 0;
}