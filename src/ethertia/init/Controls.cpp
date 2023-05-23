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
#include <ethertia/init/DebugStat.h>

//#include <ethertia/gui/screen/GuiF4Lock.h>
//#include <ethertia/gui/screen/GuiDebugV.h>
//#include <ethertia/gui/screen/GuiIngame.h>
//#include <ethertia/gui/screen/GuiScreenPause.h>

#include <ethertia/imgui/Imgui.h>

#include <ethertia/init/KeyBinding.h>

void Controls::initConsoleThread()
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





static void handleKeyPress()
{
    if (KeyBindings::KEY_ESC.isPressed())
    {
        Ethertia::isIngame() = !Ethertia::isIngame();
    }
    if (KeyBindings::KEY_FULL_VIEWPORT.isPressed())
    {
        Settings::w_Viewport_Full = !Settings::w_Viewport_Full;
    }
    if (KeyBindings::KEY_SCREENSHOT.isPressed())
    {
        Controls::saveScreenshot();
    }
    if (KeyBindings::KEY_FULLSCREEN.isPressed())
    {
        Ethertia::getWindow().toggleFullscreen();
    }
    if (KeyBindings::KEY_COMMAND.isPressed())
    {
        Settings::w_Console_FocusInput = true;
    }
    if (KeyBindings::KEY_DEBUG_INFO.isPressed())
    {
        bool dbg = !Dbg::dbg_TextInfo;
        Dbg::dbg_TextInfo =
        Dbg::dbg_WorldBasis =
        Dbg::dbg_ViewBasis =
        Dbg::dbg_ViewGizmo = dbg;
    }

    // Follow are Gameplay keys.
    if (!Ethertia::isIngame() || !Ethertia::getWorld())
        return;

    HitCursor& cur = Ethertia::getHitCursor();
    EntityPlayer& player = *Ethertia::getPlayer();
    bool isCreativeMode = player.getGamemode() == Gamemode::CREATIVE;
    bool isSurvivalMode = player.getGamemode() == Gamemode::SURVIVAL;

    const Material* hitCellMtl = nullptr;
    if (cur.cell && cur.cell->mtl) {
        hitCellMtl = cur.cell->mtl;
    }

    // Pick Terrain
    if (hitCellMtl && KeyBindings::KEY_G_PICK.isPressed() && isCreativeMode)
    {
        ItemStack stack(cur.cell->mtl->m_MaterialItem, 1);
        stack.moveTo(player.getHoldingItem());
    }

    // Break Terrain
    World* world = Ethertia::getWorld();
    if (hitCellMtl && KeyBindings::KEY_G_ATTACK.isKeyDown())
    {
        if (isSurvivalMode)
        {
            cur.cell_breaking_time += Ethertia::getDelta();// * Controls::gDigSpeedMultiplier;

            float fullDigTime = cur.cell->mtl->m_Hardness;
            Dbg::dbg_CurrCellBreakingFullTime = fullDigTime;
            if (cur.cell_breaking_time >= fullDigTime)
            {
                // Do Dig
                const Material* mtl = cur.cell->mtl;
                glm::vec3 pos_drop = cur.position + cur.normal * 0.2f; // +norm prevents fall down

                if (mtl == Materials::LEAVES)
                {
                    int rnd = Timer::timestampMillis();
                    if (rnd % 2 == 0) world->dropItem(pos_drop, ItemStack(Items::STICK, 1));
                    if (rnd % 4 == 0) world->dropItem(pos_drop, ItemStack(Items::APPLE, 1));
                    if (rnd % 4 == 0) world->dropItem(pos_drop, ItemStack(Items::LIME, 1));
                }
                else
                {
                    world->dropItem(pos_drop, ItemStack(mtl->m_MaterialItem, 1));
                }

                cur.cell->set_nil();
                world->invalidateCellFp(cur.cell_position, 3);
                world->requestRemodel(cur.cell_position);
                cur.cell = nullptr;


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


                // clear hit-cell state.
                cur.cell_breaking_time = 0;
            }
        }

        float CurrTime = Ethertia::getPreciseTime();
        static float s_LastTimeBreakTerrain = 0;
        if (isCreativeMode && (s_LastTimeBreakTerrain + Settings::gInterval_BreakingTerrain_CreativeMode) <= CurrTime)
        {
            s_LastTimeBreakTerrain = CurrTime;

            // Break Instant and No Drops.
            glm::vec3 p = cur.position;
            float n = cur.brushSize;
            for (int dx = std::floor(-n); dx <= std::ceil(n); ++dx) {
                for (int dz = std::floor(-n); dz <= std::ceil(n); ++dz) {
                    for (int dy = std::floor(-n); dy <= std::ceil(n); ++dy) {
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

    } else {
        cur.cell_breaking_time = 0;
    }


    // Use Item (could Place Terrain)
    if (KeyBindings::KEY_G_USE.isPressed())
    {
        ItemStack& stack = player.getHoldingItem();

        if (!stack.empty())
        {
            for (ItemComponent* comp : stack.item()->m_Components)
            {
                comp->onUse();
            }
            --stack.m_Amount;
        }

        // Right click two object and link them
        if (cur.hitEntity && !cur.hitTerrain)
        {
            static Entity* firstEntity = nullptr;
            static Entity* secondEntity = nullptr;

            if (firstEntity)
            {
                secondEntity = cur.hitEntity;

                btTransform frameInA, frameInB;
                frameInA = firstEntity->m_Rigidbody->getWorldTransform();
                frameInB = secondEntity->m_Rigidbody->getWorldTransform();

                btTransform relTrans = frameInA.inverseTimes(frameInB);

                auto * fixedConstraint = new btFixedConstraint(*firstEntity->m_Rigidbody, *secondEntity->m_Rigidbody, relTrans, btTransform::getIdentity());
                world->m_DynamicsWorld->addConstraint(fixedConstraint, true);

                firstEntity = nullptr;
                secondEntity = nullptr;
            }
            else
            {
                firstEntity = cur.hitEntity;
            }
        }
    }


    // Drop Item
    if (KeyBindings::KEY_G_DROPITEM.isPressed())
    {
        ItemStack& stack = player.getHoldingItem();
        if (!stack.empty())
        {
            ItemStack drop;

            bool dropAll = Ethertia::getWindow().isCtrlKeyDown();
            stack.moveTo(drop, dropAll ? stack.amount() : 1);

            Ethertia::getWorld()->dropItem(player.position(), drop,player.getViewDirection() * 3.0f);
        }
    }


    // Hotbar Scrolling / Key Switch.
    if (KeyBindings::KEY_G_HOTBAR1.isPressed()) player.m_HotbarSlot = 0;
    if (KeyBindings::KEY_G_HOTBAR2.isPressed()) player.m_HotbarSlot = 1;
    if (KeyBindings::KEY_G_HOTBAR3.isPressed()) player.m_HotbarSlot = 2;
    if (KeyBindings::KEY_G_HOTBAR4.isPressed()) player.m_HotbarSlot = 3;
    if (KeyBindings::KEY_G_HOTBAR5.isPressed()) player.m_HotbarSlot = 4;



    static Entity*      s_HoldingEntity = nullptr;  // use this persistent instead of get hitEntity every time, prevents lost of tracking
    static glm::vec3    s_HoldingEntityRelPos;  // precisely hold the hitPoint & hitDistance
    static float        s_HoldingEntityDist;

    //Constrain the object directly in front of the camera at all times
    if (KeyBindings::KEY_G_HOLD.isPressed())
    {
        if (s_HoldingEntity) {
            s_HoldingEntity = nullptr;
        } else {
            if (cur.hitEntity && !cur.hitTerrain) {
                s_HoldingEntity = cur.hitEntity;
                s_HoldingEntityRelPos = cur.position - cur.hitEntity->position();
                s_HoldingEntityDist = cur.length;
            }
        }
    }
    // (tmp) update for HoldingEntity above
    if (s_HoldingEntity)
    {
        btVector3 cameraPosition = Mth::btVec3(Ethertia::getCamera().position);
        btVector3 cameraForward = Mth::btVec3(Ethertia::getCamera().direction);
        btVector3 objectPosition = cameraPosition + cameraForward * s_HoldingEntityDist - Mth::btVec3(s_HoldingEntityRelPos);
        s_HoldingEntity->m_Rigidbody->getWorldTransform().setOrigin(objectPosition);
        // seems this have not rotate 'correctly.?' if you want rotation as cam-dir, maybe can use Quaternion e.g. setBasis(to_mat3(to_quat(camDir)))
//            target_entity->m_Rigidbody->getWorldTransform().setBasis(btMatrix3x3(cameraForward.getX(), 0.0f, 0.0f,
//                                                             0.0f, cameraForward.getY(), 0.0f,
//                                                             0.0f, 0.0f, cameraForward.getZ()));

//        Camera& cam = Ethertia::getCamera();
//        glm::vec3 force_pos = cam.position + cam.direction * s_HoldingEntityDist - s_HoldingEntityRelPos;
//        s_HoldingEntity->m_Rigidbody->getWorldTransform().setOrigin(Mth::btVec3(force_pos));
    }

    if (KeyBindings::KEY_G_SITTING.isPressed())
    {
        EntityDrivingSeat* entityDrivingSeat = dynamic_cast<EntityDrivingSeat*>(Ethertia::getHitCursor().hitEntity);
        if (entityDrivingSeat)
        {
            entityDrivingSeat->setDriver(Ethertia::getPlayer());
        }
    }

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

    if (!cur.hit) {
        cur.reset();
        return;
    }

    cur.position = p;
    cur.normal = n;
    cur.hitEntity = (Entity*)obj->getUserPointer();
    cur.length = glm::length(p - _p_beg);

    // Hit Terrain Cell.
    cur.hitTerrain = dynamic_cast<EntityMesh*>(cur.hitEntity);
    if (cur.hitTerrain)
    {
        glm::vec3 cp_base = glm::floor(p - n*0.01f);  // cell_pos. try get full block, shrink: p-n*Epsilon
        Cell* hitCell = &world->getCell(cp_base);
        cur.cell_position = cp_base;

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
        if (hitCell->mtl == nullptr) {
            hitCell = nullptr;
        }

        cur.cell = hitCell;
    }
}


void Controls::handleInput()
{
    Camera& camera = Ethertia::getCamera();
    Window& window = Ethertia::getWindow();
    EntityPlayer* player = Ethertia::getPlayer();

    if (window.isCloseRequested())
        Ethertia::shutdown();

    window.setMouseGrabbed(Ethertia::isIngame());
    window.setStickyKeys(!Ethertia::isIngame());


    // Hit Cursor.
    handleHitCursor();

    handleKeyPress();


    camera.position = Ethertia::getPlayer()->position();

    if (player->getGamemode() == Gamemode::SPECTATOR) {  // track camera viewMatrix only while InGame.
        camera.update(Ethertia::isIngame());
    } else {
        camera.update(true);
    }

    if (!Ethertia::isIngame())
        return;

    // Player Move.

    if (KeyBindings::KEY_G_SPRINT.isKeyDown()) {
        player->setSprint(true);
    } else if (!KeyBindings::KEY_G_FORWARD.isKeyDown()) {
        player->setSprint(false);
    }

    EntityHelicopter* helicopter = dynamic_cast<EntityHelicopter*>(Imgui::g_InspEntity);
    if (!helicopter) {
        player->move(KeyBindings::KEY_G_JUMP.isKeyDown(), KeyBindings::KEY_G_SNEAK.isKeyDown(),
                     KeyBindings::KEY_G_FORWARD.isKeyDown(), KeyBindings::KEY_G_BACK.isKeyDown(),
                     KeyBindings::KEY_G_LEFT.isKeyDown(), KeyBindings::KEY_G_RIGHT.isKeyDown());
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


    float dt = Ethertia::getDelta();
    camera.updateMovement(dt, window.getMouseDX(), window.getMouseDY(), window.isKeyDown(GLFW_KEY_Z));


    if (KeyBindings::KEY_G_CAM_DIST.isKeyDown())
        camera.len += window.getDScroll();
    camera.len = Mth::max(camera.len, 0.0f);




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
        BitmapImage fine_img(img->width(), img->height());
        img->getVerticalFlippedPixels(fine_img.pixels());

        Loader::savePNG(path, fine_img);
        delete img;
    });
}