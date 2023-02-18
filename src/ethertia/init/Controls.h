//
// Created by Dreamtowards on 2022/12/10.
//

#ifndef ETHERTIA_CONTROLS_H
#define ETHERTIA_CONTROLS_H

#include <ethertia/entity/EntityDroppedItem.h>
#include <ethertia/gui/screen/GuiF4Lock.h>

class Controls
{
public:

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
            if (e->isPressed() && e->getButton() == GLFW_MOUSE_BUTTON_RIGHT)
            {
                ItemStack& stack = Ethertia::getPlayer()->getHoldingItem();

                if (!stack.empty())
                {
                    for (ItemComponent* comp : stack.item()->m_Components)
                    {
                        comp->onUse();
                    }
                }
            }
        });

        win->eventbus().listen([](KeyboardEvent* e)
        {
            if (e->getKey() == GLFW_KEY_C) {
                static float fov;
                if (e->isPressed()) {
                    fov = RenderEngine::fov;
                    RenderEngine::fov = 20;
                } else if (e->isReleased()) {
                    RenderEngine::fov = fov;
                }
            }

            if (!e->isPressed())
                return;

            handleKeyDown(e->getKey());
        });


    }


    static void handleKeyDown(int key) {
        switch (key)
        {
            case GLFW_KEY_F1: {
                GuiIngame::Inst()->toggleVisible();
                break;
            }
            case GLFW_KEY_F2: {
                saveScreenshot();
                break;
            }
            case GLFW_KEY_F3: {
                GuiDebugV::Inst()->toggleVisible();
                break;
            }
            case GLFW_KEY_F4: {
                if (Ethertia::isIngame())
                    Ethertia::getRootGUI()->addGui(GuiF4Lock::Inst());
                else if (Ethertia::getRootGUI()->last() == GuiF4Lock::Inst())
                    Ethertia::getRootGUI()->removeLastGui();
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
                    GuiMessageList::Inst()->setVisible(true);

                    Ethertia::getRootGUI()->addGui(GuiScreenChat::Inst());
                    GuiScreenChat::Inst()->openCommandInput();
                }
                break;
            }
            case GLFW_KEY_Q: {
                if (Ethertia::isIngame()) {
                    EntityPlayer& player = *Ethertia::getPlayer();
                    ItemStack& stack = player.getHoldingItem();
                    if (!stack.empty())
                    {
                        EntityDroppedItem* eDroppedItem = new EntityDroppedItem();

                        stack.moveTo(eDroppedItem->m_DroppedItem, 1);

                        eDroppedItem->setPosition(player.getPosition());
                        eDroppedItem->applyLinearVelocity(player.getViewDirection() * 3.0f);

                        Ethertia::getWorld()->addEntity(eDroppedItem);
                    }
                }
                break;
            }
            case GLFW_KEY_F: {

                EntityPlayer* player = Ethertia::getPlayer();

                if (player->m_RidingOn) {
                    player->m_RidingOn->removeDriver();
                    player->m_RidingOn = nullptr;
                    return;
                }

                BrushCursor& cur = Ethertia::getBrushCursor();
                if (EntityVehicle* car = dynamic_cast<EntityVehicle*>(cur.hitEntity)) {
                    // if (cur.length > 10)  return;

                    player->m_RidingOn = car;
                    car->addDriver(player);
                }
                break;
            }
        }
    }


    // not accurate name.
    static void handleInput()
    {
        Camera& camera = *Ethertia::getCamera();
        Window& window = *Ethertia::getWindow();
        EntityPlayer* player = Ethertia::getPlayer();
        RenderEngine* renderEngine = Ethertia::getRenderEngine();

        float dt = Ethertia::getDelta();

        window.setMouseGrabbed(Ethertia::isIngame());
        window.setStickyKeys(!Ethertia::isIngame());
        // window.setTitle(("desp. "+std::to_string(1.0/dt)).c_str());

        if (Ethertia::isIngame())
        {

            // Player Movement.

            if (window.isKeyDown(GLFW_KEY_LEFT_CONTROL)) {
                player->setSprint(true);
            } else if (!window.isKeyDown(GLFW_KEY_W)) {
                player->setSprint(false);
            }

            if (player->m_RidingOn == nullptr) {
                player->move(window.isKeyDown(GLFW_KEY_SPACE), window.isKeyDown(GLFW_KEY_LEFT_SHIFT),
                                            window.isKeyDown(GLFW_KEY_W), window.isKeyDown(GLFW_KEY_S),
                                            window.isKeyDown(GLFW_KEY_A), window.isKeyDown(GLFW_KEY_D));
            }
            else
            {
                // Vehicle Control.
                player->m_RidingOn->move(window.isKeyDown(GLFW_KEY_SPACE), window.isKeyDown(GLFW_KEY_LEFT_SHIFT),
                                                        window.isKeyDown(GLFW_KEY_W), window.isKeyDown(GLFW_KEY_S),
                                                        window.isKeyDown(GLFW_KEY_A), window.isKeyDown(GLFW_KEY_D),
                                                        window.isKeyDown(GLFW_KEY_LEFT_BRACKET), window.isKeyDown(GLFW_KEY_RIGHT_BRACKET));
                                                        //window.isCtrlKeyDown() && window.isKeyDown(GLFW_KEY_A),window.isCtrlKeyDown() && window.isKeyDown(GLFW_KEY_D));
            }

            // Camera
            static SmoothValue smFov;
            smFov.update(dt);
            if (window.isKeyDown(GLFW_KEY_X)) {
                smFov.target += window.getMouseY() / 800.0f;
            }
            RenderEngine::fov += smFov.delta;

            camera.updateMovement(dt, window.getMouseDX(), window.getMouseDY(), window.isKeyDown(GLFW_KEY_Z), window.getDScroll());

            if (window.isKeyDown(GLFW_KEY_L))
                camera.len += window.getDScroll();
            camera.len = Mth::max(camera.len, 0.0f);



            // Hotbar
            player->m_HotbarSlot += Mth::signal(-window.getDScroll());
            player->m_HotbarSlot = Mth::clamp(player->m_HotbarSlot, 0, GuiIngame::HOTBAR_SLOT_MAX);

            for (int i = 0; i <= GuiIngame::HOTBAR_SLOT_MAX; ++i) {
                if (window.isKeyDown(GLFW_KEY_1+i))
                    player->m_HotbarSlot = i;
            }



            // Cursor
            BrushCursor& cursor = Ethertia::getBrushCursor();
            if (cursor.keepTracking && Ethertia::getWorld()) {
                glm::vec3 p, n;
                glm::vec3 pos_begin = camera.position;

                btCollisionObject* obj = nullptr;
                cursor.hit = Ethertia::getWorld()->raycast(pos_begin, pos_begin + camera.direction * 100.0f, p, n, &obj);

                if (cursor.hit) {
                    cursor.position = p;
                    cursor.normal = n;
                    cursor.hitEntity = (Entity*)obj->getUserPointer();
                    cursor.length = glm::length(p - pos_begin);
                } else {
                    cursor.reset();
                }
            }
        }


//    player->intpposition = /*Mth::lerp(Ethertia::getTimer()->getPartialTick(), player->prevposition, */player->getPosition();//);


        // RenderEngine updates.
        camera.position = player->getPosition();
        RenderEngine::matView = camera.computeViewMatrix();

        renderEngine->updateViewFrustum();
        renderEngine->updateProjectionMatrix(Ethertia::getAspectRatio());



    }



    static void initMouseDigControls() {



        Ethertia::getWindow()->eventbus().listen([=](MouseButtonEvent* e) {
            World* world = Ethertia::getWorld();
            EntityPlayer* player = Ethertia::getPlayer();

            if (e->isPressed() && world && Ethertia::isIngame()) {
                int btn = e->getButton();

                BrushCursor& cur = Ethertia::getBrushCursor();
                if (cur.hit) {
                    glm::vec3 p = cur.position;
                    float n = cur.brushSize;

                    if (btn == GLFW_MOUSE_BUTTON_1) {

                        if (GuiDebugV::g_BlockMode) {

                            Cell& c = world->getCell( p + -cur.normal*0.1f );

                            c.mtl = 0;
                            c.density = 0;

                            world->requestRemodel(p);
                            return;
                        }

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
                                        b.mtl = 0;
                                    }
                                    world->requestRemodel(p+d);
                                }
                            }
                        }
                    } else if (btn == GLFW_MOUSE_BUTTON_3) {
                        const Material* mtl = world->getCell(p - cur.normal*0.1f).mtl;
                        if (mtl && player->getGamemode() != Gamemode::SURVIVAL)
                        {
                            ItemStack stack(mtl->m_MaterialItem, 1);

                            player->getHoldingItem().clear();
                            stack.moveTo(player->getHoldingItem());
                        }
                    }
                }

            }
        });




    }


    static void escape_PauseOrBack()
    {
        if (Ethertia::getWorld())
        {
            if (Ethertia::getRootGUI()->last() == GuiIngame::Inst())
            {
                Ethertia::getRootGUI()->addGui(GuiScreenPause::Inst());  // Pause
            }
            else
            {
                Ethertia::getRootGUI()->removeLastGui();
            }
        }
        else
        {
            if (Ethertia::getRootGUI()->last() != GuiScreenMainMenu::Inst())
            {
                Ethertia::getRootGUI()->removeLastGui();
            }
        }
    }

    static void saveScreenshot()
    {
        BitmapImage* img = Ethertia::getWindow()->screenshot();

        std::string path = Strings::fmt("./screenshots/{}_{}.png", Strings::time_fmt("%Y-%m-%d_%H.%M.%S"), (Mth::frac(Ethertia::getPreciseTime())*1000.0f));
        if (Loader::fileExists(path))
            throw std::logic_error("File already existed.");

        Log::info("Screenshot saving to '{}'.\1", path);
        GuiMessageList::Inst()->addMessage(Strings::fmt("Saved screenshot to '{}'.", path));

        Ethertia::getAsyncScheduler()->addTask([img, path]() {
            BENCHMARK_TIMER;

            // vertical-flip image back to normal. due to GL feature.
            BitmapImage fine_img(img->getWidth(), img->getHeight());
            img->getVerticalFlippedPixels(fine_img.getPixels());

            Loader::savePNG(fine_img, path);
            delete img;
        });
    }

};

#endif //ETHERTIA_CONTROLS_H
