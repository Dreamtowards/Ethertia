//
// Created by Dreamtowards on 2022/4/22.
//

#pragma once

#include <glm/glm.hpp>
#include <stdexcept>
#include <format>
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


class World;
class WorldInfo;
class EntityPlayer;
class Window;

//    // ClientSide Only
//    World* GetWorld();
//    EntityPlayer* GetPlayer();
//    Window& GetWindow();
//    Timer& GetTimer();
//    Scheduler& GetScheduler();
//    Scheduler& GetAsyncScheduler();
//    HitCursor& GetHitCursor();
//    Profiler& GetProfiler();
//    Camera& GetCamera();
//
//
//    float GetDelta();  /// (previous) frame delta time in seconds.
//    float GetPreciseTime();  /// precise program-running-time in seconds. !not epoch timestamp.
//
//    bool IsIngame();  // is controlling game
//
//}

class Ethertia
{
public:
    Ethertia() = delete;

    static void LoadWorld(const std::string& savedir, const WorldInfo* worldinfo = nullptr);
    static void UnloadWorld();

    static World* GetWorld();


    // send chat message (@cmd not '/' leading) or dispatch command ('/' leading).
    static void DispatchCommand(const std::string& cmd);

    static void PrintMessage(const std::string& msg);  // print @msg on Message Box.


    static bool& IsRunning();
    static void Shutdown() { IsRunning() = false; }     // not immediately shutdown, but after this frame.


    static stdx::thread_pool& GetThreadPool();
    
    static bool InMainThread();


    static EntityPlayer* getPlayer();
    static Window& getWindow();
    static Timer& getTimer();
    static HitCursor& getHitCursor();
    static Profiler& getProfiler();
    static Camera& getCamera();

    static float getDelta();        // (previous) frame delta time in seconds.
    static float getPreciseTime();  // precise program-running-time in seconds. !not epoch timestamp.

    // is controlling the game. (mouse grabbed, wsad etc.)
    static bool& isIngame();




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



#define PROFILE_VN_CONCAT_INNR(a, b) a ## b
#define PROFILE_VN_CONCAT(a, b) PROFILE_VN_CONCAT_INNR(a, b)
#define PROFILE(x) auto PROFILE_VN_CONCAT(_profiler, __COUNTER__) = Ethertia::getProfiler().push_ap(x)
#define PROFILE_X(p, x) auto PROFILE_VN_CONCAT(_profiler, __COUNTER__) = p.push_ap(x)

