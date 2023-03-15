
视频: [SetupDev]




## Build & Run

1. Get Sources  
   https://github.com/Dreamtowards/Ethertia  
   _Download ZIP_ or `git clone`
2. Build
   > Prerequisites: gcc compiler, ucrt (universal c runtime).  
   > so if you are Windows, you need 'mingw64-ucrt' (go https://winlibs.com/, usually download: [x86_64-posix-seh-gcc-12.2.0-mingw-w64ucrt-10.0.0-r4.zip](https://github.com/brechtsanders/winlibs_mingw/releases/download/12.2.0-15.0.7-10.0.0-ucrt-r4/winlibs-x86_64-posix-seh-gcc-12.2.0-llvm-15.0.7-mingw-w64ucrt-10.0.0-r4.zip))
   - CMake Configuration
     - _Windows_: `cmake -B ./build -G "CodeBlocks - MinGW Makefiles"`
     - _MacOS_: `cmake -B ./build -G "CodeBlocks - Unix Makefiles"`
   - CMake Build  
     `cmake --build ./build`
3. Run  
   `./bin/windows-x64/Ethertia.exe`  
   [!] Requires _working directory_ at `./run` folder.  
   [!] If crashed on SIMD Level checking, just downgrade your simd_level in settings.json.






## Setup Dev

### IDE: CLion

为了进一步开发，你通常需要一个IDE 以便编写, Debug..  
这里介绍CLion, 因为它清晰, 跨平台.


1. 准备 CLion 2020.3.4  
   1. [官网下载 CLion 2020.3.4](https://www.jetbrains.com/clion/download/other.html)
   2. 'reset-eval' 无限续杯30天脚本
   3. [Visual Studio 2019 Dark 主题](https://plugins.jetbrains.com/plugin/14965-visual-studio-2019-dark-theme)
   > 选2020这个版本是因为 只需1秒运行reset-eval脚本即可无限30天续杯, 而且和最新版2022功能差别也不太大. 新版的破解就很复杂
3. 配置 CLion
   1. Toolchains 选到刚下载的 ucrt-mingw64
   2. 设置 working directory 到 `./run` 目录 (`$ProjectFileDir$/run`)
    

### Version Control: Github Desktop

相比之下这个就次要了，选你喜欢的。这里介绍 Github Desktop. 因为它独立 便携 清晰.






## How to Contribute


