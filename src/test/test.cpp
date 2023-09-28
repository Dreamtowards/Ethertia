

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


#include <chrono>
class testV
{
public:
    template<typename T>
    static T GenInt()
    {
        static T _i = std::chrono::system_clock::now().time_since_epoch().count();
        return _i;
    }

    static int InlintFunc()
    {
        return GenInt<int>();
    }

    static int InlintFunc2()
    {
        return GenInt<uint32_t>();
    }
};
int main()
{
    Log::info("A: {}", testV::InlintFunc());

    Log::info("A: {}", testV::InlintFunc());

    Log::info("A: {}", testV::InlintFunc2());
    Log::info("A: {}", testV::InlintFunc2());

    Log::info("A: {}", testV::InlintFunc());

    return 0;
    //for (float i = 0; i < 10000000; ++i)
    //{
    //    bool b = testret();
    //    if (b)
    //        Log::info("{}", b);
    //}
    //return 0;
    float f = 0;
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "===== Round " << i << "\n";

        test(f);
    }
    return (int)f;
}