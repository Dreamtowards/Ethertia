//
// Created by Thaumstrial on 2023-02-01.
//



#ifndef ETHERTIA_ENTITYVEHICLE_H
#define ETHERTIA_ENTITYVEHICLE_H

#include <ethertia/entity/Entity.h>
#include <ethertia/Ethertia.h>

class EntityVehicle : public Entity
{
    int maxPassenger = 2;
    std::vector<EntityPlayer*> passengers;
    EntityPlayer* driver = nullptr;

public:
    EntityVehicle() {

    }

    bool addPassenger(EntityPlayer* entityPlayer) {
        if (passengers.size() < maxPassenger) {
            passengers.push_back(entityPlayer);
            return true;
        }
        return false;
    }

    bool removePassenger(EntityPlayer* entityPlayer) {
        for (auto it = passengers.begin(); it != passengers.end(); ) {
            if ((*it) == entityPlayer) {
                it = passengers.erase(it);
                return true;
            } else {
                ++it;
            }
        }
        return false;
    }

    void addDriver(EntityPlayer* entityPlayer) {
        assert(driver == nullptr && "Driver already existed.");
        driver = entityPlayer;
    }

    void removeDriver() {
        assert(driver && "No driver exists.");
        driver = nullptr;
    }

};

#endif //ETHERTIA_ENTITYVEHICLE_H
