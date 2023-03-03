
Welcome to Ethertia Development Documentation.

- [Build on Your System](.)
1. Project Files Structure
2. Program Structure
3. Rendering (OpenGL)
4. Audio (OpenAL)
5. Entity System
6. Voxel Terrain System
7. Item System
8. GUI


## Project Files Structure

> - `lib/` _external libraries._
> - `run/` _runtime working directory_
>   - `assets/` _default assets_
>   - `bin/` _binaries. shared-library, executable._
>     - `darwin-x64/`
>     - `windows-x64/`
>     - `windows-arm64/`
>   - `logs/`
>   - `mods/`
>   - `saves/` _worlds / saves_
>   - `screenshots/`
>   - `settings.json` _settings (client)_
>   - `server.json` _dedicated server settings_
> - `src/` _c++ sources & headers._
> - `CMakeLists.txt`
> - `README.md`
> ...

Basically there are 3 directories here: `lib/`, `run/` and `src/`, which are external libraries, runtime working directory, and source code.

## Program Structure

The entrance is in the `ethertia/Ethertia.cpp`'s main(), as you can see, the top (highest) level flow is:

```cpp
void Ethertia::run() 
{
    start();

    while (m_Running)
    {
        runMainLoop();
    }

    destroy();
}
```

The `start()` _initializes_ the system, `runMainLoop()` keep updating every frame (rendering, physics, world), at the end `destroy()` is called for system clean up (when system shotdown).

1. Let's quick look the `start()`
    ```cpp
    void Ethertia::start()
    {
        Settings::loadSettings();  // just load settings
   
        ModLoader::loadMods();  // load & pre-init mods.
   
        // System Initialize
        m_Window = new Window(...);   // main display/window
        m_GuiRoot = new GuiRoot();    // root GUI
        RenderEngine::init();
        AudioEngine::init();
   
        // Material & Items init. (texture, mesh)
        ...
        // Network System, Controls init.
        ...
        // Other Threads (ChunkGen, ChunkMeshGen)
        ...
    }
    ```

2. It's time to dive into the `runMainLoop()`  
   there is the Core of the System.
    ```cpp
    Ethertia::runMainLoop()
    {
        m_Timer.update();
        m_Scheduler.processTasks();
   
        while (m_Timer.polltick()) 
        {
            runTick();
        }
   
        Controls::handleInput();  // glfwPollEvents();
   
        RenderEngine::renderWorld(m_World);
   
        renderGUI();
   
        m_Window->SwapBuffer();
    }
    ```

3. A glimpse at `destroy()`



## Rendering (OpenGL)

