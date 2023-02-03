//
// Created by Dreamtowards on 2023/1/18.
//

#ifndef ETHERTIA_COMMAND_H
#define ETHERTIA_COMMAND_H

#include <vector>
#include <string>

//#define DECL_COMMAND_ID(x) std::string getRegistryId() const override { return x; }

#define SendMessage(msg, ...) Ethertia::notifyMessage(Strings::fmt(msg, ##__VA_ARGS__))

class Command
{
public:
    inline static Registry<Command> REGISTRY;

    using Args = const std::vector<std::string>&;


    virtual void onCommand(Args args)  {

    }

    virtual void onComplete(Args args, int argi, std::vector<std::string>& completes) const {
    }

    static Entity* resolveEntityExpr(const std::string& expr) {
        Entity* result = nullptr;
        if (expr == "@t") {
            result = Ethertia::getBrushCursor().hitEntity;
            if (!result) Log::warn("Failed, invalid target entity");
        } else if (expr == "@s") {
            result = Ethertia::getPlayer();
        }
        assert(result);
        return result;
    }
};

#endif //ETHERTIA_COMMAND_H
