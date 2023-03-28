
# Build & Run
[[VIDEO]]()

如果您想构建源代码并运行，以便分析或修改，这里是正确的地方.  
如果您只想运行，可以前往 Releases 里下载已编译发布版本直接运行.

### 1. Download

https://github.com/Dreamtowards/Ethertia  
下载源代码仓库 (约200MB),  

### 2. Build
为了构建程序, 运行 `build_windows.bat`. (或`build_darwin.command`(macOS))
> 需要CMake

> **注意:** 对于 Windows 系统，您需要 ucrt mingw64 的开发环境，因为我们是unix系gcc编译器.  
> 您可以在 winlibs.com 上下载, 一个常见版本是:
> [x86_64-posix-seh-gcc-12.2.0-mingw-w64ucrt-10.0.0-r4.zip](https://github.com/brechtsanders/winlibs_mingw/releases/download/12.2.0-15.0.7-10.0.0-ucrt-r4/winlibs-x86_64-posix-seh-gcc-12.2.0-llvm-15.0.7-mingw-w64ucrt-10.0.0-r4.zip)


### 3. Run

构建完毕后，运行 run 文件夹里的 `run_windows.bat`.

> **注:** 程序会被构建于例 `run/bin/windows-x64/` 文件夹中, 而运行目录需要在 `run/` 文件夹下.


---

如果您想进一步了解或参与开发，可见 [Getting Dev](), [Source Overview]().