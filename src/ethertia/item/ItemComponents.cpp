//
// Created by Dreamtowards on 2023/2/18.
//


#include <ethertia/item/ItemComponents.h>

#include <ethertia/Ethertia.h>
#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/render/Window.h>
//#include <ethertia/gui/screen/GuiDebugV.h>


void ItemComponentFood::onUse()
{
    EntityPlayer* player = Ethertia::getPlayer();

    player->m_Health += m_Heal;
}

void ItemComponentMaterial::onUse()
{

    HitCursor& cur = Ethertia::getHitCursor();
    if (!cur.hit)
        return;

    glm::vec3 p = cur.position;

    Material* mtl = m_Material;

    if (mtl->m_IsVegetable || mtl->m_CustomMesh || Ethertia::getWindow().isKeyDown(GLFW_KEY_B))  // || GuiDebugV::g_BlockMode
    {
        Cell& c = Ethertia::getWorld()->getCell(p + cur.normal*0.1f);

        c.density = 0;
        c.mtl = mtl;
        c.exp_meta = 1;

        Ethertia::getWorld()->requestRemodel(p);
        return;
    }

    int n = cur.brushSize;
    AABB::forCube(n, [n, p, mtl](glm::vec3 rp) {
        Cell& b = Ethertia::getWorld()->getCell(p + rp);
        float f = n - glm::length(rp + glm::vec3(0.5));

        if (!Ethertia::getWindow().isAltKeyDown())  {
            b.density = Mth::max(b.density, f);
        }
        if (b.density >= 0.0f)
            b.mtl = mtl;

        Ethertia::getWorld()->requestRemodel(p+rp);
    });
}



void ItemComponentEntity::onUse()  {

    Entity* e = m_NewEntity();

    HitCursor& cur = Ethertia::getHitCursor();

    e->position() = cur.position + cur.normal * 1.0f;
    Ethertia::getWorld()->addEntity(e);
}



void ItemComponentToolGrapple::onUse()
{
    glm::vec3 dir = Ethertia::getCamera().direction;

    Ethertia::getPlayer()->applyLinearVelocity(dir * 24.3f);
}

void ItemComponentToolUniversalLinkTool::onUse()
{
    bool isHitTerrain = !Ethertia::getHitCursor().hitTerrain;
    Entity* hitEntity = Ethertia::getHitCursor().hitEntity;

    if (hitEntity && !isHitTerrain)
    {
        if (firstEntity)
        {
            secondEntity = hitEntity;
            secondLocation.setOrigin(Mth::btVec3(Ethertia::getHitCursor().position));

            // Reset two joints
            firstEntity = nullptr;
            firstLocation.setIdentity();

            secondEntity = nullptr;
            secondLocation.setIdentity();
        } else
        {
            firstEntity = hitEntity;
            firstLocation.setOrigin(Mth::btVec3(Ethertia::getHitCursor().position));
        }
    }
    if (firstEntity && secondEntity)
    {
        switch (currentMode) {
            case Point2Point:
            {
                auto* point2pointConstraint = new btPoint2PointConstraint(*firstEntity->m_Rigidbody, *secondEntity->m_Rigidbody, firstLocation.getOrigin(), secondLocation.getOrigin());
                Ethertia::getWorld()->m_DynamicsWorld->addConstraint(point2pointConstraint);
                break;
            }
            case Hinge:
            {
                auto* hingeConstraint = new btHingeConstraint(*firstEntity->m_Rigidbody, *secondEntity->m_Rigidbody, firstLocation, secondLocation, true);
                Ethertia::getWorld()->m_DynamicsWorld->addConstraint(hingeConstraint);
                break;
            }
            case Generic6Dof:
            {
                auto* generic6DofConstraint = new btGeneric6DofConstraint(*firstEntity->m_Rigidbody, *secondEntity->m_Rigidbody, firstLocation, secondLocation, true);
                Ethertia::getWorld()->m_DynamicsWorld->addConstraint(generic6DofConstraint);
                break;
            }
        }
    }
}