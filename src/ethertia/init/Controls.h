//
// Created by Dreamtowards on 2022/12/10.
//

#ifndef ETHERTIA_CONTROLS_H
#define ETHERTIA_CONTROLS_H

class Controls
{
public:
    inline static Entity* m_FocusedEntity = nullptr;

    static void initConsoleThread()
    {
        new std::thread([]()
        {
            Log::info("Console thread is ready");

            while (Ethertia::isRunning())
            {
                std::string line;
                std::getline(std::cin, line);

                Ethertia::dispatchCommand(line);
            }
        });
    }


    static void initControls()
    {
        Controls::initConsoleThread();

        initMouseDigControls();


        Window* win = Ethertia::getWindow();

        win->eventbus().listen([](WindowCloseEvent* e)
        {
            Ethertia::shutdown();
        });

        win->eventbus().listen([](MouseButtonEvent* e)
        {
            if (e->isPressed() && e->getButton()) {

            }
        });

        win->eventbus().listen([](KeyboardEvent* e)
        {
            if (!e->isPressed())
                return;

            switch (e->getKey())
            {
                case GLFW_KEY_F1: {
                    GuiIngame::INST->toggleVisible();
                    break;
                }
                case GLFW_KEY_F2: {
                    saveScreenshot();
                    break;
                }
                case GLFW_KEY_F3: {
                    GuiDebugV::INST->toggleVisible();
                    break;
                }
                case GLFW_KEY_F11: {
                    Ethertia::getWindow()->toggleFullscreen();
                    break;
                }
                case GLFW_KEY_ESCAPE: {
                    escape_PauseOrBack();
                    break;
                }
                case GLFW_KEY_SLASH: {
                    if (Ethertia::isIngame()) {
                        GuiMessageList::INST->setVisible(true);
                        Ethertia::getRootGUI()->addGui(GuiScreenChat::INST);
                    }
                    break;
                }
            }
        });


    }


    // not accurate name.
    static void handleInput()
    {
        Camera& camera = *Ethertia::getCamera();
        Window& window = *Ethertia::getWindow();
        Entity* player = Ethertia::getPlayer();
        RenderEngine* renderEngine = Ethertia::getRenderEngine();

        float dt = Ethertia::getDelta();

        window.setMouseGrabbed(Ethertia::isIngame());
        window.setStickyKeys(!Ethertia::isIngame());
        // window.setTitle(("desp. "+std::to_string(1.0/dt)).c_str());
        if (Ethertia::isIngame())
        {
            // Player Movement.

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

            static SmoothValue smFov;
            smFov.update(dt);
            if (window.isKeyDown(GLFW_KEY_X)) {
                smFov.target += window.getMouseY() / 800.0f;
            }
            RenderEngine::fov += smFov.delta;

            camera.updateMovement(dt, window.getMouseDX(), window.getMouseDY(), window.isKeyDown(GLFW_KEY_Z), window.getDScroll());
        }
//    player->intpposition = /*Mth::lerp(Ethertia::getTimer()->getPartialTick(), player->prevposition, */player->getPosition();//);




        // RenderEngine updates.
        camera.position = player->getPosition();
        renderEngine->viewMatrix = camera.computeViewMatrix();

        renderEngine->updateViewFrustum();
        renderEngine->updateProjectionMatrix(Ethertia::getAspectRatio());




        // Cursor
        BrushCursor& cursor = Ethertia::getBrushCursor();
        if (cursor.keepTracking) {
            glm::vec3 p, n;

            void* usrptr = nullptr;
            cursor.hit = Ethertia::getWorld()->raycast(camera.position, camera.position + camera.direction * 100.0f, p, n, &usrptr);

            cursor.position = cursor.hit ? p : glm::vec3(0.0f);

            cursor.hitEntity = (Entity*)usrptr;
        }

    }



    static void initMouseDigControls() {



        Ethertia::getWindow()->eventbus().listen([=](MouseButtonEvent* e) {
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
                                    b.density = b.density - Mth::max(0.0f, f);
                                    if (f > 0 && b.density < 0 //&& b.id == Materials::LEAVES
                                        ) {
                                        b.id = 0;
                                    }
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


    static void escape_PauseOrBack()
    {
        if (Ethertia::isIngame())
        {
            Ethertia::getRootGUI()->addGui(GuiScreenPause::INST);  // Pause
        }
        else
        {
            assert(Ethertia::getRootGUI()->last() != GuiIngame::INST);
            Ethertia::getRootGUI()->removeLastGui();
        }
    }

    static void saveScreenshot()
    {
        BitmapImage* img = Ethertia::getWindow()->screenshot();

        std::string path = Strings::fmt("./screenshots/{}_{}.png", Strings::time_fmt("%Y-%m-%d_%H.%M.%S"), (Mth::frac(Ethertia::getPreciseTime())*1000.0f));
        if (Loader::fileExists(path))
            throw std::logic_error("File already existed.");

        Log::info("Screenshot saving to '{}'.\1", path);
        GuiMessageList::INST->addMessage(Strings::fmt("Saved screenshot to '{}'.", path));

        Ethertia::getAsyncScheduler()->addTask([img, path]() {
            BenchmarkTimer _tm;

            // vertical-flip image back to normal. due to GL feature.
            BitmapImage fine_img(img->getWidth(), img->getHeight());
            img->getVerticalFlippedPixels(fine_img.getPixels());

            Loader::savePNG(&fine_img, path);
            delete img;
        });
    }

};

#endif //ETHERTIA_CONTROLS_H
