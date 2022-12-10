//
// Created by Dreamtowards on 2022/12/10.
//

#ifndef ETHERTIA_CONTROLS_H
#define ETHERTIA_CONTROLS_H

class Controls
{
public:



    static void updateMovement() {
        Window& window = *Ethertia::getWindow();

        static bool sprint = false;

        float speed = 0.5;
        if (window.isKeyDown(GLFW_KEY_LEFT_CONTROL)) sprint = true;
        if (sprint) speed = 3.8;
        float yaw = Ethertia::getCamera()->eulerAngles.y;

        glm::vec3 vel(0);
        if (window.isKeyDown(GLFW_KEY_W)) vel += Mth::angleh(yaw) * speed;
        if (window.isKeyDown(GLFW_KEY_S)) vel += Mth::angleh(yaw + Mth::PI) * speed;
        if (window.isKeyDown(GLFW_KEY_A)) vel += Mth::angleh(yaw + Mth::PI / 2) * speed;
        if (window.isKeyDown(GLFW_KEY_D)) vel += Mth::angleh(yaw - Mth::PI / 2) * speed;

        if (window.isShiftKeyDown())          vel.y -= speed;
        if (window.isKeyDown(GLFW_KEY_SPACE)) vel.y += speed;

        Ethertia::getPlayer()->applyLinearVelocity(vel);

        if (!window.isKeyDown(GLFW_KEY_W)) {
            sprint = false;
        }
    }


    // not accurate name.
    static void handleInput()
    {
        Camera& camera = *Ethertia::getCamera();
        Window& window = *Ethertia::getWindow();
        Entity* player = Ethertia::getPlayer();
        RenderEngine* renderEngine = Ethertia::getRenderEngine();

        float dt = Ethertia::getDelta();

        if (Ethertia::isIngame()) {
            updateMovement();
            camera.updateMovement(dt, window.getMouseDX(), window.getMouseDY(), window.isKeyDown(GLFW_KEY_Z), window.getDScroll());
        }
        window.setMouseGrabbed(Ethertia::isIngame());
        window.setStickyKeys(!Ethertia::isIngame());
        window.setTitle(("desp. "+std::to_string(1.0/dt)).c_str());

//    player->intpposition = /*Mth::lerp(Ethertia::getTimer()->getPartialTick(), player->prevposition, */player->getPosition();//);
        camera.compute(player->getPosition(), renderEngine->viewMatrix);

        renderEngine->updateViewFrustum();
        renderEngine->updateProjectionMatrix(Ethertia::getAspectRatio());

        BrushCursor& brushCursor = Ethertia::getBrushCursor();
        if (brushCursor.keepTracking) {
            glm::vec3 p, n;
            if (Ethertia::getWorld()->raycast(camera.position, camera.position + camera.direction * 100.0f, p, n)) {
                brushCursor.hit = true;
                brushCursor.position = p;
            } else {
                brushCursor.hit = false;
                brushCursor.position = glm::vec3(0.0f);
            }
        }

    }




    static void initMouseDigControls() {

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
                    float n = cur.brushSize;

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

#endif //ETHERTIA_CONTROLS_H
