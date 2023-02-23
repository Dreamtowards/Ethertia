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
    for (int dx = -n; dx <= n; ++dx) {
        for (int dz = -n; dz <= n; ++dz) {
            for (int dy = -n; dy <= n; ++dy) {
                glm::vec3 d(dx, dy, dz);

                Cell& b = Ethertia::getWorld()->getCell(p + d);
                float f = n - glm::length(d);

                if (!Ethertia::getWindow()->isAltKeyDown())  {
                    b.density = Mth::max(b.density, f);
                }
                if (b.density >= 0.0f)
                    b.mtl = mtl;

                Ethertia::getWorld()->requestRemodel(p+d);
            }
        }
    }
}



void ItemComponentEntity::onUse()  {

    Entity* e = m_NewEntity();

    HitCursor& cur = Ethertia::getHitCursor();

    e->setPosition(cur.position + cur.normal * 1.0f);
    Ethertia::getWorld()->addEntity(e);
}



void ItemComponentGrapple::onUse()
{
    glm::vec3 dir = Ethertia::getCamera()->direction;

    Ethertia::getPlayer()->applyLinearVelocity(dir * 24.3f);
}