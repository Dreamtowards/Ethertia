//
// Created by Dreamtowards on 2022/10/6.
//

#ifndef ETHERTIA_ENTITYPLAYER_H
#define ETHERTIA_ENTITYPLAYER_H

#include <ethertia/entity/Entity.h>
#include <ethertia/entity/player/Gamemode.h>

class EntityPlayer : public Entity
{
public:
    int gamemode = Gamemode::SURVIVAL;

    bool flying = false;

    // for Spectator-Gamemode setback.
    btCollisionShape* ref_Shape = nullptr;

    EntityPlayer() : Entity(50.0f, "entity/cube.obj") {

        rigidbody->setActivationState(DISABLE_DEACTIVATION);
    }

    void setFlying(bool f) {
        flying = f;

        if (f) {
            rigidbody->setDamping(0.96, 0.96);
            rigidbody->setGravity(btVector3(0, 0, 0));
        } else {
            rigidbody->setDamping(0, 0);
            rigidbody->setGravity(btVector3(0, -10, 0));
        }
    }
    bool isFlying() {
        return flying;
    }

    void switchGamemode(int mode) {

        if (mode == Gamemode::SPECTATOR) {
            ref_Shape = rigidbody->getCollisionShape();
            rigidbody->setCollisionShape(new btEmptyShape());
        } else {
            if (ref_Shape) {
                rigidbody->setCollisionShape(ref_Shape);
            }
        }


    }
};

#endif //ETHERTIA_ENTITYPLAYER_H
