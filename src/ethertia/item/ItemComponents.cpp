//
// Created by Dreamtowards on 2023/2/18.
//


#include <ethertia/item/ItemComponents.h>

#include <ethertia/Ethertia.h>
#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/render/Window.h>
#include <ethertia/gui/screen/GuiDebugV.h>


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

    if (mtl->m_IsVegetable || mtl->m_CustomMesh || GuiDebugV::g_BlockMode || Ethertia::getWindow()->isKeyDown(GLFW_KEY_B))
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

        if (!Ethertia::getWindow()->isAltKeyDown())  {
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

    e->setPosition(cur.position + cur.normal * 1.0f);
    Ethertia::getWorld()->addEntity(e);
}



void ItemComponentToolGrapple::onUse()
{
    glm::vec3 dir = Ethertia::getCamera()->direction;

    Ethertia::getPlayer()->applyLinearVelocity(dir * 24.3f);
}