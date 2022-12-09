//
// Created by Dreamtowards on 2022/4/29.
//

#ifndef ETHERTIA_INIT_H
#define ETHERTIA_INIT_H

#include "MaterialTextures.h"

class Init
{
public:

    static void initialize()
    {
        MaterialTextures::init();

        // init Texture::UNIT.
        BitmapImage img(1, 1, new u32[1]{(u32)~0});
        Texture::UNIT = Loader::loadTexture(&img);
    }


    static void initSomeTests() {

//    EntityCar* car = new EntityCar();
//    world->addEntity(car);
//    car->setPosition({10, 10, -10});


//        EntityRaycastCar* raycastCar = new EntityRaycastCar();
//    raycastCar->setPosition({0, 5, -10});
//    world->addEntity(raycastCar);

//        EventBus::EVENT_BUS.listen([&, raycastCar](KeyboardEvent* e) {
//            if (e->isPressed()) {
//                int key = e->getKey();
//                if (key == GLFW_KEY_ESCAPE) {
//                } else if (isIngame()) {
//                    if (key == GLFW_KEY_SLASH) {
//                        getRootGUI()->addGui(GuiScreenChat::INST);
//                    } else if (key == GLFW_KEY_G) {
//                        raycastCar->m_vehicle->applyEngineForce(100, 2);
//                        raycastCar->m_vehicle->applyEngineForce(100, 3);
//
//                        raycastCar->m_vehicle->setBrake(100, 2);
//                        raycastCar->m_vehicle->setBrake(100, 3);
//
//
//                        raycastCar->m_vehicle->setSteeringValue(0, 0);
//                        raycastCar->m_vehicle->setSteeringValue(0, 1);
//
//                        Log::info("Force");
//                    }
//                }
//            }
//        });
        EventBus::EVENT_BUS.listen([=](MouseButtonEvent* e) {
            World* world = Ethertia::getWorld();
            Window* window = Ethertia::getWindow();
            if (e->isPressed() && world && Ethertia::isIngame()) {
                static u8 placingBlock = Materials::GRASS;

                int btn = e->getButton();

                BrushCursor& cur = Ethertia::getBrushCursor();
                if (cur.hit) {
                    glm::vec3 p = cur.position;
                    float n = cur.size;

//                glm::vec3 v = (p - camera.position) * 0.8f;
//                v.y += glm::length(v) * 0.4f;
//                player->applyLinearVelocity(v);

                    if (btn == GLFW_MOUSE_BUTTON_1) {
                        for (int dx = floor(-n); dx <= ceil(n); ++dx) {
                            for (int dz = floor(-n); dz <= ceil(n); ++dz) {
                                for (int dy = floor(-n); dy <= ceil(n); ++dy) {
                                    glm::vec3 d(dx, dy, dz);

                                    Cell& b = world->getCell(p + d);
                                    float f = n - glm::length(d);

                                    // b.id = placingBlock;
                                    b.density = b.density - Mth::max(0.0f, n - glm::length(d));
                                    world->requestRemodel(p+d);
                                }
                            }
                        }
                    } else if (btn == GLFW_MOUSE_BUTTON_2) {
                        for (int dx = -n; dx <= n; ++dx) {
                            for (int dz = -n; dz <= n; ++dz) {
                                for (int dy = -n; dy <= n; ++dy) {
                                    glm::vec3 d(dx, dy, dz);

                                    Cell& b = world->getCell(p + d);
                                    float f = n - glm::length(d);

                                    if (!window->isAltKeyDown())  {
                                        b.density = Mth::max(b.density, f);
                                    }
                                    if (b.density >= 0.0f)
                                        b.id = placingBlock;

                                    world->requestRemodel(p+d);
                                }
                            }
                        }
                        world->requestRemodel(p);
                    } else if (btn == GLFW_MOUSE_BUTTON_3) {
                        placingBlock = world->getCell(p).id;
                        Log::info("Picking: {}", (int)placingBlock);
                    }
                }

            }
        });




    }
};

#endif //ETHERTIA_INIT_H
