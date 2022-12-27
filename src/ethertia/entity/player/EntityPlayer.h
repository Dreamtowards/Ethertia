//
// Created by Dreamtowards on 2022/10/6.
//

#ifndef ETHERTIA_ENTITYPLAYER_H
#define ETHERTIA_ENTITYPLAYER_H

#include <ethertia/entity/Entity.h>
#include <ethertia/entity/player/Gamemode.h>

class EntityPlayer : public Entity
{
    int m_Gamemode = Gamemode::SURVIVAL;

    bool m_Flying = false;

    int m_Health = 20;

public:


    inline static btCollisionShape* g_SpectatorShape = new btEmptyShape();

    // for Spectator-Gamemode setback.
    btCollisionShape* m_CachedShape = nullptr;


    EntityPlayer() {

        initRigidbodyConvexModel(50.0f, std::move(*Loader::loadOBJ(Loader::loadAssetsStr("entity/cube.obj"))));

        m_Rigidbody->setActivationState(DISABLE_DEACTIVATION);
    }

    void setFlying(bool f) {
        m_Flying = f;

        if (f) {
            m_Rigidbody->setDamping(0.96, 0.96);
            m_Rigidbody->setGravity(btVector3(0, 0, 0));
        } else {
            m_Rigidbody->setDamping(0, 0);
            m_Rigidbody->setGravity(btVector3(0, -10, 0));
        }
    }
    bool isFlying() const {
        return m_Flying;
    }

    void switchGamemode(int mode)
    {
        if (mode == Gamemode::SPECTATOR)
        {
            m_CachedShape = m_Rigidbody->getCollisionShape();
            m_Rigidbody->setCollisionShape(g_SpectatorShape);
        }
        else if (m_CachedShape)
        {
            m_Rigidbody->setCollisionShape(m_CachedShape);
        }
    }

};

#endif //ETHERTIA_ENTITYPLAYER_H
