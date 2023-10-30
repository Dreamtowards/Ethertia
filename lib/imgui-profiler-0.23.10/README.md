# TimelineProfiler
Simple CPU/GPU profiler with ImGui HUD
GPU profiler only supports D3D12

![](Image.jpg)

## Usage

### Setup

Add files to project:
- Profiler.h
- Profiler.cpp
- ProfilerWindow.cpp
- IconsFontAwesome4.h
- fontawesome-webfont.ttf

Add the icon font and merge it with your main font
```c++
ImFontConfig fontConfig;
fontConfig.MergeMode = true;
fontConfig.GlyphMinAdvanceX = 15.0f
static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FA, 15.0f, &fontConfig, icon_ranges);
```

### CPU Profiler

#### Initialize

```c++
// Initialize
gCPUProfiler.Initialize(historySize, maxNumEvents);
```

#### Shutdown

```c++
// Shutdown
gCPUProfiler.Shutdown();
```

#### Each frame
```c++
// Call at the start of each frame
PROFILE_FRAME()
```

**CPU Event**

`PROFILE_CPU_SCOPE()` to add a CPU event. Optionally specify a custom name


**Registering a thread (optional)**

`PROFILE_REGISTER_THREAD(name)` to register a thread.

The registration order of threads will define the order in the timeline
If a thread is not registered, it will lazy-register when an event is created first.


### GPU Profiler

If you want to use the GPU profiler, initialize and shutdown as following, providing your command queues.

#### Initialize

```c++
// Initialize
Span<ID3D12CommandQueue*> queues;
gGPUProfiler.Initialize(d3dDevice, queues, historySize, frameLatency, maxNumEvents, maxActiveCommandLists);
```

#### Shutdown

```c++
// Shutdown
gGPUProfiler.Shutdown();
```

#### Each frame
```c++
// Call at the start of each frame
PROFILE_FRAME()
```

#### Each `ExecuteCommandLists` (not optional!)

```c++
ID3D12CommandQueue* queue = ...;
Span<ID3D12CommandList*> cmdlists = ...;
PROFILE_EXECUTE_COMMANDLISTS(queue, cmdlists);
queue->ExecuteCommandLists(cmdlists.data(), cmdlists.size());
```

#### Adding events

`PROFILE_GPU_SCOPE(commandlist, name)` to add a GPU event.

Specify the ID3D12GraphicsCommandList, and optionally a name.