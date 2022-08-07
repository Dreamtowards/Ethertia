
<div class="float-end" style="margin-top: 120px;">

<a class="d-block btn btn-primary px-5 py-3 " href="/docs/getting_started/hello_world">
    Download Elytra
    <small class="d-block mt-1">Released on July 22, 2022</small>
</a>

<a class="d-block btn btn-primary px-5 py-3 mt-3" href="/docs/getting_started/hello_world">
    Get Started
</a>

<div class="mt-3 rounded border" style="background: #f8f8f8; padding: 12px">
    <span style="font-size: 25px; margin-top: 3px" class="float-end me-1">13%</span>
    <span style="font-weight: 500;">Elytra 0.0.1 progress</span>
    <div style="display:inline-block; width: 65%;background: #ddd">
        <div style="height: 4px; width: 13%; background: #0071e3"></div>
    </div>
</div>

<div class="mt-3 text-muted" style="font-size: 14px">
    <span class="fs-small">'Spicy' Developers: </span><br>
    - <a href="https://elytra.dev/~pris" class="text-muted text-decoration-none">Eldrine Le Prismarine</a><br>
    - Edward D. Rolinsen<br>
</div>

<!--div class="mt-5">
<p class="fw-semibold mb-2">Release 0.0.1 <small>on Jun 18, 2022</small></p>
<span class="fs-small">- ffi interop with cplusplus.</span>
</div-->

<div class="mt-5">
    <a class="twitter-timeline" href="//twitter.com/ethertia?ref_src=twsrc%5Etfw" style="width: 200px;">Tweets by ethertia</a>
</div>

</div>

<div markdown="1" style="width: 68%">

<div markdown="1" style="font-size: 16px; margin-top: 76px; color: #4d4d4d; "> 

<b class="text-black">Elytra</b> is an Low-Level, VM-base, multi-platform Programming Language for Core System Development of Voxel Game _Ethertia_.  
It pursuits Simplicity, Consistence and Essence. strong interoperablity/FFI with Cplusplus are provided for Native Functionality and Performance. LLVM might be join in to play in serval years later.

Elytra Runtime (Macedure VM) and Compiler are written in Cplusplus and currently supports Windows, Linux, MacOS.

<!--Elytra is licenced under the zlib/libpng license., and is developing & maintaining by Eldrine Le Prismarine, Edward D. Rolinsen, et, al. and the Community.-->

</div>

<br><br><br><br><br><br>

## Sample code 1
Source:
```
namespace ethertia::client;

using std::String;
using std::System;
using std::io::Console;
using std::util::ArrayList;
using std::mth::vec3;
using std::mth::vec<N, i32> as ivec<N>;

using ethertia::client::Window;
using ethertia::client::gui::GuiRoot;
using ethertia::client::render::Camera;
using ethertia::entity::player::EntityPlayer;
using ethertia::util::Timer;
using ethertia::util::Log;
using ethertia::util::concurrent::Scheduler;
using ethertia::world::WorldClient;

class Ethertia {

    static bool running;
    
    static Window& window;
    static Timer& timer;
    static Scheduler& scheduler;
    
    static WorldClient& world;
    static EntityPlayer& player;
    static GuiRoot& rootGUI;
    static Camera& camera;
    
    void run() {
        
        start();
        
        while (running) 
        {
            runMainLoop();
        }
        
        destroy();
    }
    
    void start() {
    
        ArrayList<String> args = System::args();
        
        Console::writeln();
        Log::info();
        // ...
    
    }

}
```
IR:
```
$ elytra build eth.et --ir
```
Runtime Dump:
```
$ elytra run eth.o --debug-verbo ethertoa::client::Ethertia::run

```

</div>


<!--p class="pt-5 mb-1" style="font-size: 48px">Pursuit of Simplicity</p>
<p>我们想创建出 有意识 清楚 简单 一致的程序</p-->
