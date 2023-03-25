//
// Created by Dreamtowards on 2023/3/25.
//


// EntryPoint
// CNS. 一个单独的入口点，但没用到。单独入口点的好处是 入口可能是平台相关的 分离入口点则可以跨平台 分离平台与系统本身。
//      但目前根本用不到不同的入口点 所以避免提早优化就不用单独的入口点了

int main(int argc, char** argv, char** env)
{
    Ethertia::Run();

    return 0;
}