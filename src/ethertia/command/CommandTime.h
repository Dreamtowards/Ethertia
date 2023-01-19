//
// Created by Dreamtowards on 2023/1/18.
//

#ifndef ETHERTIA_COMMANDTIME_H
#define ETHERTIA_COMMANDTIME_H

#include <string>

class CommandTime : public Command
{
public:

    // time set <19:12/3/3am/3AM/5:32aM>

    void onCommand(const std::vector<std::string>& args) override
    {
        if (args[1] == "set")
        {
            // midnight=0, sunrise=5, day=7, noon=12, sunset=18, night=19
            float daytime = Strings::daytime(args[2]);

            Ethertia::getWorld()->m_DayTime = daytime;

            Ethertia::notifyMessage(Strings::fmt("Time set to {}.", Strings::daytime(daytime, true)));
        }
        else if (args[1] == "spd")
        {
            float f = std::stof(args[2]);
            Ethertia::getWorld()->m_DayTimeScale = f;

            Ethertia::notifyMessage(Strings::fmt("DayTime already set to {} seconds per day", f));
        }
    }

};

#endif //ETHERTIA_COMMANDTIME_H
