//
// Created by Dreamtowards on 2023/1/18.
//

#ifndef ETHERTIA_COMMAND_H
#define ETHERTIA_COMMAND_H

#define DECL_COMMAND_ID(x) std::string getRegistryId() const override { return x; }

class Command
{
public:
    inline static Registry<Command> REGISTRY;

    virtual void onCommand(const std::vector<std::string>& args)  {

    }

    virtual void onComplete() {

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
