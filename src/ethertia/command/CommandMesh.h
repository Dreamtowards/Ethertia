//
// Created by Dreamtowards on 2023/1/18.
//

#ifndef ETHERTIA_COMMANDMESH_H
#define ETHERTIA_COMMANDMESH_H


class CommandMesh : public Command
{
public:

    void onCommand(const std::vector<std::string>& args) override
    {
        EntityPlayer* player = Ethertia::getPlayer();

        if (args[1] == "new")
        {
            EntityMesh* entity = new EntityMesh();
            Ethertia::getWorld()->addEntity(entity);
            entity->setPosition(player->getPosition());

            const VertexBuffer& vbuf = *MaterialMeshes::STOOL;
            entity->setMesh(EntityMesh::createMeshShape(vbuf.vertexCount(), vbuf.positions.data()));
            entity->updateModel(Loader::loadModel(&vbuf));

            Ethertia::notifyMessage(Strings::fmt("EntityMesh created."));
        }
        else
        {
            Entity* target = Ethertia::getBrushCursor().hitEntity;
            if (!target) {
                Ethertia::notifyMessage("failed, no target entity.");
                return;
            }

            if (args[1] == "set")
            {
                const std::string& path = args[2];
                if (!Loader::fileExists(path)){
                    Ethertia::notifyMessage(Strings::fmt("No mesh file on: ", path));
                    return;
                }

                EntityMesh* eMesh = (EntityMesh*)target;

                VertexBuffer* vbuf = Loader::loadOBJ(path.c_str());
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
    }

};

#endif //ETHERTIA_COMMANDMESH_H
