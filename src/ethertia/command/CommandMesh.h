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
            entity->position() = player->position();

            const VertexData* vtx = MaterialMeshes::STOOL;
            entity->setMesh(EntityMesh::CreateMeshShape(vtx));
            entity->updateModel(Loader::loadVertexData(vtx));

            _SendMessage("EntityMesh created.");
        }
        else
        {
            Entity* target = Ethertia::getHitCursor().hitEntity;
            if (!target) {
                _SendMessage("failed, no target entity.");
                return;
            }

            if (args[1] == "set")
            {
                const std::string& path = args[2];
                if (!Loader::fileExists(path)){
                    _SendMessage("No mesh file on: ", path);
                    return;
                }

                EntityMesh* eMesh = (EntityMesh*)target;

                VertexData* vtx = Loader::loadOBJ(path.c_str());
                eMesh->updateModel(Loader::loadVertexData(vtx));
                eMesh->setMesh(EntityMesh::CreateMeshShape(vtx));

                _SendMessage("Mesh updated.");
            }
            else if (args[1] == "diff")
            {
                const std::string& path = args[2];
                if (!Loader::fileExists(path)){
                    _SendMessage("No texture file on: ", path);
                    return;
                }

                EntityMesh* entity = (EntityMesh*)Ethertia::getHitCursor().hitEntity;
                entity->m_DiffuseMap = Loader::loadTexture(path);

                _SendMessage("Texture updated.");
            }
            else if (args[1] == "delete")
            {
                Ethertia::getWorld()->removeEntity(target);

                _SendMessage("Entity deleted.");
            }

        }
    }

};

#endif //ETHERTIA_COMMANDMESH_H
