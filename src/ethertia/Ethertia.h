//
// Created by Dreamtowards on 2022/4/22.
//

#pragma once

#include <glm/glm.hpp>
#include <stdx/thread_pool.h>

#include <ethertia/render/Camera.h>
#include <ethertia/init/HitCursor.h>
#include <ethertia/util/Timer.h>
#include <ethertia/util/Profiler.h>
#include <ethertia/util/Registry.h>

#define ET_VERSION_MAJOR 0
#define ET_VERSION_MINOR 0
#define ET_VERSION_PATCH 10
#define ET_VERSION_SNAPSHOT "2023.10a"
#define ET_VERSION_DATE "2023.10.02"


class World;
class WorldInfo;


class Ethertia
{
public:
    Ethertia() = delete;

    static void LoadWorld(const std::string& savedir, const WorldInfo* worldinfo = nullptr);
    static void UnloadWorld();

    static World* GetWorld();

    static bool& IsRunning();
    static void Shutdown() { IsRunning() = false; }     // not immediately shutdown, but after this frame.

    static stdx::thread_pool& GetThreadPool();
    static bool InMainThread();

    static float GetDelta();        // (previous) frame delta time in seconds.
    static float GetPreciseTime();  // precise program-running-time in seconds. !not epoch timestamp.


    static Timer& GetTimer();
    static Profiler& GetProfiler();
    static Camera& GetCamera();
    static HitCursor& GetHitCursor();   // HitResult of World Raycast
    //static Entity GetPlayer();

    
    static bool& isIngame();        // is controlling the game. (mouse grabbed, wsad etc.)


    // send chat message (@cmd not '/' leading) or dispatch command ('/' leading).
    static void DispatchCommand(const std::string& cmd);
    static void PrintMessage(const std::string& msg);  // print @msg on Message Box.


    static const std::string GetVersion(bool fullname = false);     // e.g. "0.0.3", "23w14c *0.0.2" fullname: "Ethertia Alpha 23w14c *0.0.2"


    struct Viewport
    {
        float x, y, width, height;

        // width/height. return 0 if height==0.
        float AspectRatio() const { if (height==0) return 0; return width/height; }
        float right() const { return x+width; }
        float bottom() const { return y+height; }
    };
    // game viewport. useful when game required to be rendered in a specific area.
    static Ethertia::Viewport GetViewport();


};



#define ET_MACRO_CONCAT_INNR(a, b) a ## b
#define ET_MACRO_CONCAT(a, b) ET_MACRO_CONCAT_INNR(a, b)
#define ET_PROFILE_(x) auto ET_MACRO_CONCAT(_profiler, __COUNTER__) = Ethertia::GetProfiler().push_ap(x)
#define ET_PROFILE(x, p) auto ET_MACRO_CONCAT(_profiler, __COUNTER__) = p.push_ap(x)

