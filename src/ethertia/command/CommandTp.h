//
// Created by Dreamtowards on 2023/1/18.
//

#ifndef ETHERTIA_COMMANDTP_H
#define ETHERTIA_COMMANDTP_H



class CommandTp : public Command
{
public:
    void onCommand(const std::vector<std::string>& args) override
    {
        Entity* src = Ethertia::getPlayer();
        int argc = args.size();
        if (argc == 4) { // /tp <x> <y> <z>
            src->setPosition(Mth::vec3(&args[1]));
        } else if (argc == 5) {
            src = resolveEntityExpr(args[1]);
            src->setPosition(Mth::vec3(&args[2]));
        } else if (argc == 3) {
            src = resolveEntityExpr(args[1]);
            Entity* dst = resolveEntityExpr(args[2]);
            src->setPosition(dst->getPosition());
        }
        _SendMessage("Teleported");
    }
};

#endif //ETHERTIA_COMMANDTP_H
