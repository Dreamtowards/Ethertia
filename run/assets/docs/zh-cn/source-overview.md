
# Source Overview

In this article, 

## Files Structure

```
- lib/              # external libraries
- run/              # runtime working directory
  - assets/         # default assets
  - bin/            # binaries. shared-libraries, executables.
    - darwin-x64/
    - windows-x64/
    - windows-arm64/
 - logs/
 - mods/
 - saves/           # worlds / saves
 - screenshots/
 - settings.json    # client configs
 - server.json      # dedicated server configs
- src/              # c++ sources & headers.
- CMakeLists.txt
- README.md
  ...
```

Basically there are 3 directories here: `lib/`, `run/` and `src/`, which are external libraries, runtime working directory, and source code.


## Program Structure


The entry point is in the `src/ethertia/Ethertia.cpp`'s main().

```cpp
static void Run() 
{
    Init();
    
    while (g_Running) 
    {
        RunMainLoop();
    }
    
    Destroy();
}
```