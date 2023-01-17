//
// Created by Dreamtowards on 2023/1/2.
//

#ifndef ETHERTIA_COMMANDS_H
#define ETHERTIA_COMMANDS_H

#include <map>

class Commands
{
public:

    using CommandHandler = std::function<void(const std::vector<std::string>& args)>;

    inline static std::map<std::string, CommandHandler> COMMANDS;


    static void initCommands()
    {
        COMMANDS["heal"] = [](auto& args)
        {
            float heal = 20.0;
            if (args.size() > 2) {
                heal = std::stof(args[2]);
            }

            Ethertia::getPlayer()->m_Health = heal;
        };

        COMMANDS["tp"] = [](auto& args)
        {
            Entity* src = Ethertia::getPlayer();
            int argc = args.size();
            if (argc == 4) { // /tp <x> <y> <z>
                src->setPosition(Mth::vec3(&args[1]));
            } else if (argc == 5) {
                src = Commands::resolveEntityExpr(args[1]);
                src->setPosition(Mth::vec3(&args[2]));
            } else if (argc == 3) {
                src = Commands::resolveEntityExpr(args[1]);
                Entity* dst = Commands::resolveEntityExpr(args[2]);
                src->setPosition(dst->getPosition());
            }
            Ethertia::notifyMessage("Teleported");
        };

        COMMANDS["gamemode"] = [](auto& args)
        {
            int mode = std::stoi(args[1]);

            Ethertia::getPlayer()->switchGamemode(mode);

            Ethertia::notifyMessage(Strings::fmt("Gamemode has been set to {}", Gamemode::nameOf(mode)));
        };

        COMMANDS["fly"] = [](auto& args)
        {
            EntityPlayer* player = Ethertia::getPlayer();
            player->setFlying(!player->isFlying());

            Ethertia::notifyMessage(Strings::fmt("Flymode tuned to {}", player->isFlying()));
        };

        COMMANDS["connect"] = [](auto& args)
        {
            std::string hostname = args[1];
            int port = std::stoi(args[2]);

            NetworkSystem::connect(hostname, port);
        };

        // give <target> <item_id> [amount]
        COMMANDS["give"] = [](auto& args)
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
        };
        COMMANDS["entity"] = [](auto& args)
        {
            EntityPlayer* player = Ethertia::getPlayer();

            if (args[1] == "new")
            {
                if (args[2] == "mesh")
                {
                    EntityMesh* entity = new EntityMesh();
                    entity->setPosition(player->getPosition());

                    VertexBuffer* vbuf = Loader::loadOBJ_("entity/plane.obj");
                    entity->setMesh(EntityMesh::createMeshShape(vbuf->vertexCount(), vbuf->positions.data()));
                    entity->updateModel(Loader::loadModel(vbuf));

                    Ethertia::getWorld()->addEntity(entity);

                    Ethertia::notifyMessage(Strings::fmt("EntityMesh created."));
                }
            }
            else
            {
                Entity* target = Ethertia::getBrushCursor().hitEntity;
                if (!target) {
                    Ethertia::notifyMessage("failed, no target entity.");
                    return;
                }

                if (args[1] == "mesh")
                {
                    const std::string& path = args[2];
                    if (!Loader::fileExists(path)){
                        Ethertia::notifyMessage(Strings::fmt("No mesh file on: ", path));
                        return;
                    }

                    EntityMesh* eMesh = (EntityMesh*)target;

                    VertexBuffer* vbuf = Loader::loadOBJ_(path.c_str());
                    eMesh->updateModel(Loader::loadModel(vbuf));
                    eMesh->setMesh(EntityMesh::createMeshShape(vbuf->vertexCount(), vbuf->positions.data()));

                    Ethertia::notifyMessage("Mesh updated.");
                }
                else if (args[1] == "diff")
                {
                    const std::string& path = args[2];
                    if (!Loader::fileExists(path)){
                        Ethertia::notifyMessage(Strings::fmt("No texture file on: ", path));
                        return;
                    }

                    EntityMesh* entity = (EntityMesh*)Ethertia::getBrushCursor().hitEntity;
                    entity->m_DiffuseMap = Loader::loadTexture(Loader::loadPNG(Loader::loadFile(path)));

                    Ethertia::notifyMessage("Texture updated.");
                }
                else if (args[1] == "delete")
                {

                    Ethertia::getWorld()->removeEntity(target);

                    Ethertia::notifyMessage("Entity deleted.");
                }

            }
        };
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

#endif //ETHERTIA_COMMANDS_H
