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
#define ET_VERSION_SNAPSHOT 
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



    struct Viewport
    {
        float x, y, width, height;

        // width/height. return 0 if height==0.
        float getAspectRatio() const {
            if (height==0) return 0;
            return width/height;
        }
        float right() const { return x+width; }
        float bottom() const { return y+height; }

    };

    // game viewport. useful when game required to be rendered in a specific area.
    static const Ethertia::Viewport& getViewport();

    struct Version
    {
        inline static const int major = 0;
        inline static const int minor = 0;
        inline static const int revision = 3;
        inline static const char* snapshot = "23w10";  // release: nullptr.

        static const std::string& version_name() {
            static std::string _Inst = snapshot ? std::format("{} *{}.{}.{}", snapshot, major, minor, revision)
                                                : std::format("{}.{}.{}", major, minor, revision);
            return _Inst;
        }
        static const std::string& name() {
            static std::string _Inst = std::format("Ethertia Alpha {}", version_name());
            return _Inst;
        }
    };


};



#define ET_MACRO_CONCAT_INNR(a, b) a ## b
#define ET_MACRO_CONCAT(a, b) ET_MACRO_CONCAT_INNR(a, b)
#define ET_PROFILE_(x) auto ET_MACRO_CONCAT(_profiler, __COUNTER__) = Ethertia::GetProfiler().push_ap(x)
#define ET_PROFILE(x, p) auto ET_MACRO_CONCAT(_profiler, __COUNTER__) = p.push_ap(x)

