

#include <ethertia/util/BenchmarkTimer.h>
#include <ethertia/util/Log.h>

#include <ethertia/util/Math.h>


void test(float& v)
{
    const float N = 10 * 1000000;

    Log::info("{} {}; {} {}", Math::Floor16(3), Math::Floor16(-3), Math::Mod16(3), Math::Mod16(-3));
    Log::info("{} {}; {} {}", std::floor(3.0f / 16.0f) * 16, std::floor(-3.0f / 16) * 16, Math::Mod(3.0f, 16.0f), Math::Mod(-3.0f, 16.0f));
    {
        BENCHMARK_TIMER;
        for (float i = 0; i < N; ++i)
        {
            ++v;
        }
    }
    {
        BENCHMARK_TIMER;
        for (float i = 0; i < N; ++i)
        {
            v += Math::Floor16(i);
        }
    }
    {
        BENCHMARK_TIMER;
        for (float i = 0; i < N; ++i)
        {
            v += Math::Mod16(i);;
        }
    }
    {
        BENCHMARK_TIMER;
        for (float i = 0; i < N; ++i)
        {
            v += Math::Floor(i / 16) * 16;;
        }
    }
    {
        BENCHMARK_TIMER;
        for (float i = 0; i < N; ++i)
        {
            v += std::floor(i / 16) * 16;;
        }
    }
    {
        BENCHMARK_TIMER;
        for (float i = 0; i < N; ++i)
        {
            v += Math::Mod(i, 16.0f);
        }
    }
}


int main()
{
    float f = 0;
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "===== Round " << i << "\n";

        test(f);
    }
    return (int)f;
}