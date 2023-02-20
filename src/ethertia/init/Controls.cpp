//
// Created by Dreamtowards on 2023/2/19.
//

#include <ethertia/init/Controls.h>

#include <ethertia/Ethertia.h>
#include <ethertia/render/Window.h>
#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/entity/EntityDroppedItem.h>

#include <ethertia/gui/screen/GuiF4Lock.h>
#include <ethertia/gui/screen/GuiDebugV.h>
#include <ethertia/gui/screen/GuiIngame.h>
#include <ethertia/gui/screen/GuiScreenPause.h>


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



void handleMouseButton(MouseButtonEvent* e)
{
    World* world = Ethertia::getWorld();
    EntityPlayer* player = Ethertia::getPlayer();
    HitCursor& cur = Ethertia::getHitCursor();

    if (!(world && Ethertia::isIngame()))
        return;


    if (!e->isPressed()) {
        cur.cell_breaking_time = 0;
        return;
    }

    // Use Item
    if (e->getButton() == GLFW_MOUSE_BUTTON_RIGHT)
    {
        ItemStack& stack = player->getHoldingItem();

        if (!stack.empty())
        {
            for (ItemComponent* comp : stack.item()->m_Components)
            {
                comp->onUse();
            }
        }
    }

        // MouseButton Middle, Picking.
//        if (e->getButton() == GLFW_MOUSE_BUTTON_3)
//        {
//            const Material* mtl = world->getCell(p - cur.normal*0.1f).mtl;
//            if (mtl && player->getGamemode() != Gamemode::SURVIVAL)
//            {
//                ItemStack stack(mtl->m_MaterialItem, 1);
//
//                player->getHoldingItem().clear();
//                stack.moveTo(player->getHoldingItem());
//            }
//        }



    if (!cur.hit)
        return;

//    glm::vec3 p = cur.position;
//    float n = cur.brushSize;
//    if (e->getButton() == GLFW_MOUSE_BUTTON_LEFT) {
//
//        if (GuiDebugV::g_BlockMode) {
//
//            Cell& c = world->getCell( p + -cur.normal*0.1f );
//
//            c.mtl = 0;
//            c.density = 0;
//
//            world->requestRemodel(p);
//            return;
//        }
//
//        for (int dx = floor(-n); dx <= ceil(n); ++dx) {
//            for (int dz = floor(-n); dz <= ceil(n); ++dz) {
//                for (int dy = floor(-n); dy <= ceil(n); ++dy) {
//                    glm::vec3 d(dx, dy, dz);
//
//                    Cell& b = world->getCell(p + d);
//                    float f = n - glm::length(d);
//
//                    // b.id = placingBlock;
//                    b.density = b.density - Mth::max(0.0f, f);
//                    if (f > 0 && b.density < 0 //&& b.id == Materials::LEAVES
//                            ) {
//                        b.mtl = 0;
//                    }
//                    world->requestRemodel(p+d);
//                }
//            }
//        }
//    }


}



static void handleKeyDown(KeyboardEvent* e) {
    int key = e->getKey();

    if (key == GLFW_KEY_C) {
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


    switch (key)
    {
        case GLFW_KEY_F1: {
            GuiIngame::Inst()->toggleVisible();
            break;
        }
        case GLFW_KEY_F2: {
            Controls::saveScreenshot();
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
            if (Ethertia::getWorld()) {
                if (Ethertia::getRootGUI()->last() != GuiIngame::Inst()) {
                    Ethertia::getRootGUI()->removeLastGui();
                } else {
                    Ethertia::getRootGUI()->addGui(GuiScreenPause::Inst());  // Pause
                }
            } else {
                if (Ethertia::getRootGUI()->last() != GuiScreenMainMenu::Inst()) {
                    Ethertia::getRootGUI()->removeLastGui();
                }
            }
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
                    ItemStack drop;

                    bool dropAll = Ethertia::getWindow()->isCtrlKeyDown();
                    stack.moveTo(drop, dropAll ? stack.amount() : 1);

                    Ethertia::getWorld()->dropItem(player.getPosition(), drop,player.getViewDirection() * 3.0f);
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

            HitCursor& cur = Ethertia::getHitCursor();
            if (EntityVehicle* car = dynamic_cast<EntityVehicle*>(cur.hitEntity)) {
                // if (cur.length > 10)  return;

                player->m_RidingOn = car;
                car->addDriver(player);
            }
            break;
        }
    }
}


void Controls::initControls()
{
    initConsoleThread();

    Ethertia::getWindow()->eventbus().listen([](WindowCloseEvent* e)
    {
        Ethertia::shutdown();
    });

    Ethertia::getWindow()->eventbus().listen(handleMouseButton);

    Ethertia::getWindow()->eventbus().listen(handleKeyDown);
}

void handleHitCursor()
{
    World* world = Ethertia::getWorld();
    Camera& camera = *Ethertia::getCamera();

    HitCursor& cur = Ethertia::getHitCursor();
    if (!(cur.keepTracking && world))
        return;

    glm::vec3 p, n;
    glm::vec3 _p_beg = camera.position;

    btCollisionObject* obj = nullptr;
    cur.hit = Ethertia::getWorld()->raycast(_p_beg, _p_beg + camera.direction * 100.0f, p, n, &obj);

    if (cur.hit)
    {
        cur.position = p;
        cur.normal = n;
        cur.hitEntity = (Entity*)obj->getUserPointer();
        cur.length = glm::length(p - _p_beg);

        // Hit Terrain Cell.
        cur.hitTerrain = dynamic_cast<EntityMesh*>(cur.hitEntity);
        if (cur.hitTerrain)
        {
            glm::vec3 cp_base = glm::floor(p);  // cell_pos
            Cell* hitCell = &world->getCell(cp_base);
            cur.cell_position = cp_base;

            // check full block, shrink: p-n*Epsilon
            if (!hitCell->mtl) {
                Cell& c = world->getCell(p - n*0.1f);
                if (c.mtl && c.exp_meta == 1) {  // is cube
                    hitCell = &c;
                }
            }

            // check smooth terrain. 8 corners, sel max density.
            if (!hitCell->mtl && hitCell->exp_meta == 0) {  // nil or smooth-mtl.
                glm::vec3 max_p{INFINITY};
                float max_f = 0;
                for (int i = 0; i < 8; ++i) {
                    glm::vec3 cp = cp_base + SurfaceNetsMeshGen::VERT[i];  // corners. cell_pos.
                    const Cell& c = world->getCell(cp);
                    if (c.mtl && c.density > max_f) {
                        max_f = c.density;
                        max_p = cp;
                    }
                }
                if (max_p.x != INFINITY) {
                    cur.cell_position = max_p;
                    hitCell = &world->getCell(max_p);
                }
            }

            cur.cell = hitCell;
            if (hitCell->mtl == nullptr || hitCell != cur.cell) {
                cur.cell_breaking_time = 0;  // reset breaking_time. hitting target changed.
            }
        }
    }
    else
    {
        cur.reset();
    }


    if (Ethertia::isIngame() && Ethertia::getWindow()->isMouseLeftDown() && cur.cell && cur.cell->mtl)
    {
        cur.cell_breaking_time += Ethertia::getDelta();

        float fullDigTime = cur.cell->mtl->m_Hardness;
        if (cur.cell_breaking_time >= fullDigTime)
        {
            // Do Dig
            world->dropItem(cur.position + cur.normal * 0.2f, // +norm prevents fall down
                            ItemStack(cur.cell->mtl->m_MaterialItem, 1));
            cur.cell->set_nil();
            world->requestRemodel(cur.cell_position);

            // clear hit-cell state.
            cur.cell = nullptr;
            cur.cell_breaking_time = 0;
        }
    } else {
        cur.cell_breaking_time = 0;
    }
}

void Controls::handleContinuousInput()
{
    Camera& camera = *Ethertia::getCamera();
    Window& window = *Ethertia::getWindow();
    EntityPlayer* player = Ethertia::getPlayer();

    float dt = Ethertia::getDelta();

    window.setMouseGrabbed(Ethertia::isIngame());
    window.setStickyKeys(!Ethertia::isIngame());

    // Hit Cursor.
    handleHitCursor();

    if (!Ethertia::isIngame())
        return;


    // Player Move.

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
    {   // Vehicle Control.
        player->m_RidingOn->move(window.isKeyDown(GLFW_KEY_SPACE), window.isKeyDown(GLFW_KEY_LEFT_SHIFT),
                                 window.isKeyDown(GLFW_KEY_W), window.isKeyDown(GLFW_KEY_S),
                                 window.isKeyDown(GLFW_KEY_A), window.isKeyDown(GLFW_KEY_D),
                                 window.isKeyDown(GLFW_KEY_LEFT_BRACKET), window.isKeyDown(GLFW_KEY_RIGHT_BRACKET));
    }


    // Camera Move
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



    // Hotbar Scrolling / Key Switch.
    player->m_HotbarSlot += Mth::signal(-window.getDScroll());
    player->m_HotbarSlot = Mth::clamp(player->m_HotbarSlot, 0, GuiIngame::HOTBAR_SLOT_MAX);

    for (int i = 0; i <= GuiIngame::HOTBAR_SLOT_MAX; ++i) {
        if (window.isKeyDown(GLFW_KEY_1+i))
            player->m_HotbarSlot = i;
    }


}


void Controls::saveScreenshot()
{
    BitmapImage* img = Ethertia::getWindow()->screenshot();

    std::string path = Strings::fmt("./screenshots/{}_{}.png", Strings::time_fmt("%Y-%m-%d_%H.%M.%S"), (Mth::frac(Ethertia::getPreciseTime())*1000.0f));
    if (Loader::fileExists(path))
        throw std::logic_error("File already existed.");

    Log::info("Screenshot saving to '{}'.\1", path);
    Ethertia::notifyMessage(Strings::fmt("Saved screenshot to '{}'.", path));

    Ethertia::getAsyncScheduler()->addTask([img, path]() {
        BENCHMARK_TIMER;

        // vertical-flip image back to normal. due to GL feature.
        BitmapImage fine_img(img->getWidth(), img->getHeight());
        img->getVerticalFlippedPixels(fine_img.getPixels());

        Loader::savePNG(fine_img, path);
        delete img;
    });
}