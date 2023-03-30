//
// Created by Dreamtowards on 2023/2/19.
//

#include <ethertia/init/Controls.h>

#include <ethertia/Ethertia.h>
#include <ethertia/render/Window.h>
#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/entity/EntityDroppedItem.h>
#include <ethertia/entity/EntityMesh.h>
#include <ethertia/render/chunk/SurfaceNetsMeshGen.h>
#include "Settings.h"
#include "ethertia/entity/component/EntityDrivingSeat.h"
#include "ethertia/entity/component/EntityPropeller .h"

//#include <ethertia/gui/screen/GuiF4Lock.h>
//#include <ethertia/gui/screen/GuiDebugV.h>
//#include <ethertia/gui/screen/GuiIngame.h>
//#include <ethertia/gui/screen/GuiScreenPause.h>

#include <ethertia/imgui/Imgui.h>

#include <ethertia/init/KeyBinding.h>

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
            --stack.m_Amount;
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

    glm::vec3 p = cur.position;
    float n = cur.brushSize;
    if (e->getButton() == GLFW_MOUSE_BUTTON_LEFT) {

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
//        AABB::forCube(dig_size, [world, cp, dig_size](glm::vec3 rp)
//        {
//            Cell& c = world->getCell(cp+rp);
//
//            float f = std::max(0.0f, dig_size - glm::length(rp + glm::vec3(0.5f)));
//
//            c.density -= f;
////                    if (c.density < 0) { c.mtl = 0; }
//
//            world->requestRemodel(cp+rp);
//        });
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
    }


}



static void handleKeyPress()
{
    if (ImGui::IsKeyPressed(KeyBindings::KEY_ESC.key()))
    {
        Ethertia::isIngame() = !Ethertia::isIngame();
    }
    else if (ImGui::IsKeyPressed(KeyBindings::KEY_FULL_VIEWPORT.key()))
    {
        Settings::ws_FullViewport = !Settings::ws_FullViewport;
    }
    else if (ImGui::IsKeyPressed(KeyBindings::KEY_SCREENSHOT.key()))
    {
        Controls::saveScreenshot();
    }
    else if (ImGui::IsKeyPressed(KeyBindings::KEY_FULLSCREEN.key()))
    {
        Ethertia::getWindow().toggleFullscreen();
    }
    else if (ImGui::IsKeyPressed(KeyBindings::KEY_COMMAND.key()))
    {
        Settings::w_Console_FocusInput = true;
    }
    else if (ImGui::IsKeyPressed(KeyBindings::KEY_DROPITEM.key()))
    {
        if (Ethertia::isIngame()) {
            EntityPlayer& player = *Ethertia::getPlayer();
            ItemStack& stack = player.getHoldingItem();
            if (!stack.empty())
            {
                ItemStack drop;

                bool dropAll = Ethertia::getWindow().isCtrlKeyDown();
                stack.moveTo(drop, dropAll ? stack.amount() : 1);

                Ethertia::getWorld()->dropItem(player.position(), drop,player.getViewDirection() * 3.0f);
            }
        }
    }


}


void Controls::initControls()
{
    initConsoleThread();

    Ethertia::getWindow().eventbus().listen(handleMouseButton);

}

void handleHitCursor()
{
    World* world = Ethertia::getWorld();
    Camera& camera = Ethertia::getCamera();

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
            glm::vec3 cp_base = glm::floor(p - n*0.01f);  // cell_pos
            Cell* hitCell = &world->getCell(cp_base);
            cur.cell_position = cp_base;

//            // check full block, shrink: p-n*Epsilon
//            if (!hitCell->mtl) {
//                Cell& c = world->getCell(p - n*0.1f);
//                if (c.mtl && c.exp_meta == 1) {  // is cube
//                    hitCell = &c;
//                }
//            }

            // check smooth terrain. for 8 corners, sel a valid cell that closest to the hit point.
            if (!hitCell->mtl || hitCell->isSmoothMtl()) {  // nil or smooth-mtl.
                glm::vec3 sel_p{INFINITY};
                float min_dist = INFINITY;
                for (int i = 0; i < 8; ++i) {
                    glm::vec3 cp = cp_base + SurfaceNetsMeshGen::VERT[i];  // corners. cell_pos.
                    const Cell& c = world->getCell(cp);
                    float dist = glm::length2(cp-p);
                    if (c.mtl && dist < min_dist) {
                        min_dist = dist;
                        sel_p = cp;
                    }
                }
                if (sel_p.x != INFINITY) {
                    cur.cell_position = sel_p;
                    hitCell = &world->getCell(sel_p);
                }
            }

            if (hitCell->mtl == nullptr || hitCell != cur.cell) {
                cur.cell_breaking_time = 0;  // reset breaking_time. hitting target changed.
            }
            cur.cell = hitCell;
        }


        // Hit EntityDrivingSeat Cell.
        EntityDrivingSeat* drivingSeat = dynamic_cast<EntityDrivingSeat*>(cur.hitEntity);
        if (drivingSeat)
        {
            drivingSeat->setDriver(Ethertia::getPlayer());
        }
        EntityPropeller* propeller = dynamic_cast<EntityPropeller*>(cur.hitEntity);
        if (propeller)
        {
            propeller->rotate();
        }
    }
    else
    {
        cur.reset();
    }


    if (Ethertia::isIngame() && Ethertia::getWindow().isMouseLeftDown() && cur.cell && cur.cell->mtl)
    {
        cur.cell_breaking_time += Ethertia::getDelta() * Controls::gDigSpeedMultiplier;

        float fullDigTime = cur.cell->mtl->m_Hardness;
        if (cur.cell_breaking_time >= fullDigTime)
        {
            // Do Dig
//            world->dropItem(cur.position + cur.normal * 0.2f, // +norm prevents fall down
//                            ItemStack(cur.cell->mtl->m_MaterialItem, 1));
//            cur.cell->set_nil();
//            world->invalidateCellFp(cur.cell_position, 3);
//            world->requestRemodel(cur.cell_position);


//            if (!Ethertia::getPlayer()->getHoldingItem().empty())
//            {
//                glm::vec3 cp = cur.cell_position;
//                float dig_size = cur.brushSize;
//                AABB::forCube(dig_size, [world, cp, dig_size](glm::vec3 rp)
//                {
//                    Cell& c = world->getCell(cp+rp);
//
//                    float f = std::max(0.0f, dig_size - glm::length(rp + glm::vec3(0.5f)));
//
//                    c.density -= f;
////                    if (c.density < 0) { c.mtl = 0; }
//
//                    world->requestRemodel(cp+rp);
//                });
//            }
//            else
//            {
//                world->digCell(cur.cell_position);
//            }


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
    Camera& camera = Ethertia::getCamera();
    Window& window = Ethertia::getWindow();
    EntityPlayer* player = Ethertia::getPlayer();

    if (window.isCloseRequested())
        Ethertia::shutdown();

    float dt = Ethertia::getDelta();

    window.setMouseGrabbed(Ethertia::isIngame());
    window.setStickyKeys(!Ethertia::isIngame());

    // Hit Cursor.
    handleHitCursor();

    handleKeyPress();





    camera.update(Ethertia::isIngame());

    if (!Ethertia::isIngame())
        return;

    camera.position = Ethertia::getPlayer()->position();

    // Player Move.

    if (window.isKeyDown(GLFW_KEY_LEFT_CONTROL)) {
        player->setSprint(true);
    } else if (!window.isKeyDown(GLFW_KEY_W)) {
        player->setSprint(false);
    }

    EntityHelicopter* helicopter = dynamic_cast<EntityHelicopter*>(Imgui::g_InspEntity);
    if (!helicopter) {
        player->move(window.isKeyDown(GLFW_KEY_SPACE), window.isKeyDown(GLFW_KEY_LEFT_SHIFT),
                     window.isKeyDown(GLFW_KEY_W), window.isKeyDown(GLFW_KEY_S),
                     window.isKeyDown(GLFW_KEY_A), window.isKeyDown(GLFW_KEY_D));
    }
    else
    {
        helicopter->move(window.isKeyDown(GLFW_KEY_SPACE), window.isKeyDown(GLFW_KEY_LEFT_SHIFT),
                                 window.isKeyDown(GLFW_KEY_W), window.isKeyDown(GLFW_KEY_S),
                                 window.isKeyDown(GLFW_KEY_A), window.isKeyDown(GLFW_KEY_D),
                                 window.isKeyDown(GLFW_KEY_LEFT_BRACKET), window.isKeyDown(GLFW_KEY_RIGHT_BRACKET));
    }


    // Camera Move
//    static SmoothValue smFov;
//    smFov.update(dt);
//    if (window.isKeyDown(GLFW_KEY_X)) {
//        smFov.target += window.getMouseY() / 800.0f;
//    }
//    RenderEngine::fov += smFov.delta;

    camera.updateMovement(dt, window.getMouseDX(), window.getMouseDY(), window.isKeyDown(GLFW_KEY_Z), window.getDScroll());

    if (window.isKeyDown(GLFW_KEY_L))
        camera.len += window.getDScroll();
    camera.len = Mth::max(camera.len, 0.0f);



    int HOTBAR_SLOT_MAX = 9;
    // Hotbar Scrolling / Key Switch.
    player->m_HotbarSlot += Mth::signal(-window.getDScroll());
    player->m_HotbarSlot = Mth::clamp(player->m_HotbarSlot, 0, HOTBAR_SLOT_MAX);

    for (int i = 0; i <= HOTBAR_SLOT_MAX; ++i) {
        if (window.isKeyDown(GLFW_KEY_1+i))
            player->m_HotbarSlot = i;
    }


}


void Controls::saveScreenshot()
{
    BitmapImage* img = Ethertia::getWindow().screenshot();

    std::string path = Strings::fmt("./screenshots/{}_{}.png", Strings::time_fmt(-1, "%Y-%m-%d_%H.%M.%S"), (Mth::frac(Ethertia::getPreciseTime())*1000.0f));
    if (Loader::fileExists(path))
        throw std::logic_error("File already existed.");

    Log::info("Screenshot saving to '{}'.\1", path);
    Ethertia::notifyMessage(Strings::fmt("Saved screenshot to '{}'.", path));

    Ethertia::getAsyncScheduler().addTask([img, path]() {
        BENCHMARK_TIMER;

        // vertical-flip image back to normal. due to GL feature.
        BitmapImage fine_img(img->getWidth(), img->getHeight());
        img->getVerticalFlippedPixels(fine_img.getPixels());

        Loader::savePNG(fine_img, path);
        delete img;
    });
}