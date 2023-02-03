//
// Created by Dreamtowards on 2023/1/2.
//

#ifndef ETHERTIA_COMMANDS_H
#define ETHERTIA_COMMANDS_H

#include <map>

#include <ethertia/command/Command.h>

#include <ethertia/command/CommandTp.h>
#include <ethertia/command/CommandMesh.h>
#include <ethertia/command/CommandTime.h>
#include <ethertia/command/CommandWorldEdit.h>


class Commands
{
public:


    class CommandHeal : public Command {
    public:
        void onCommand(const std::vector<std::string>& args) override
        {
            float heal = 10.0;
            if (args.size() > 2) {
                heal = std::stof(args[2]);
            }

            Ethertia::getPlayer()->m_Health = heal;
        }
    };
    class CommandGamemode : public Command
    {
    public:
        void onCommand(const std::vector<std::string>& args) override
        {
            int mode = std::stoi(args[1]);

            Ethertia::getPlayer()->switchGamemode(mode);

            Ethertia::notifyMessage(Strings::fmt("Gamemode has been set to {}", Gamemode::nameOf(mode)));
        }
    };
    class CommandFly : public Command
    {
    public:
        void onCommand(const std::vector<std::string>& args) override
        {
            EntityPlayer* player = Ethertia::getPlayer();
            player->setFlying(!player->isFlying());

            Ethertia::notifyMessage(Strings::fmt("Flymode tuned to {}", player->isFlying()));
        }
    };
    class CommandGive : public Command
    {
    public:
        // give <target> <item_id> [amount]
        void onCommand(const std::vector<std::string>& args) override
        {
            EntityPlayer* target = (EntityPlayer*)resolveEntityExpr(args[1]);

            std::string id = args[2];

            if (!Item::REGISTRY.has(id)) {
                Ethertia::notifyMessage(Strings::fmt("No such item id \"{}\".", id));
                return;
            }

            int amount = 1;
            if (args.size() > 3) {
                amount = std::stoi(args[3]);
            }

            ItemStack stack(Item::REGISTRY.get(id), amount);

            target->m_Inventory.putItemStack(stack);

            Ethertia::notifyMessage(Strings::fmt("Given {} x{}", id, amount));
        }
    };



#define REGISTER_CMD(VAR, id, t) inline static Command* VAR = Command::REGISTRY.regist(id, t);

    REGISTER_CMD(TP, "tp", new CommandTp());
    REGISTER_CMD(HEAL, "heal", new CommandHeal());
    REGISTER_CMD(GAMEMODE, "gamemode", new CommandGamemode());
    REGISTER_CMD(FLY, "fly", new CommandFly());
    REGISTER_CMD(MESH, "mesh", new CommandMesh());
    REGISTER_CMD(TIME, "time", new CommandTime());

    REGISTER_CMD(WE_SET, "/set", new CommandWESet());
    REGISTER_CMD(WE_POS1, "/pos1", new CommandWEPos1());
    REGISTER_CMD(WE_POS2, "/pos2", new CommandWEPos2());

};

#endif //ETHERTIA_COMMANDS_H
