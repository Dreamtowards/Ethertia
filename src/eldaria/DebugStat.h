//
// Created by Dreamtowards on 2023/3/21.
//

#ifndef ETHERTIA_DEBUGSTAT_H
#define ETHERTIA_DEBUGSTAT_H


class DebugStat
{
public:

    inline static bool dbg_PauseImgui = false;
    inline static int dbg_FPS = 0;

    // return true: once per second. update fps value.
    static bool _fps_frame(double curr_sec) {
        static int fps_sec = 0;
        ++fps_sec;

        static double last_fps_time = 0;
        if (last_fps_time == 0) {
            last_fps_time = curr_sec;
        } else if (curr_sec - last_fps_time >= 1.0f) {
            dbg_FPS = fps_sec;
            fps_sec = 0;
            last_fps_time = curr_sec;
            return true;
        }
        return false;
    }
};

using Dbg = DebugStat;

#endif //ETHERTIA_DEBUGSTAT_H
