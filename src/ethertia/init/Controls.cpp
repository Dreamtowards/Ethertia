//
// Created by Dreamtowards on 2023/2/19.
//

#include "Controls.h"

#include <ethertia/Ethertia.h>
#include <ethertia/render/Window.h>
#include <ethertia/imgui/Imgui.h>
#include <ethertia/imgui/Imw.h>
#include <ethertia/init/Settings.h>
#include <ethertia/util/Math.h>
#include <ethertia/util/Log.h>

#include <ethertia/init/KeyBinding.h>



static void _HandleKeyPress()
{
    if (KeyBindings::KEY_ESC.isPressed())
    {
        Ethertia::isIngame() = !Ethertia::isIngame();
    }
    if (KeyBindings::KEY_FULL_VIEWPORT.isPressed())
    {
        ImwGame::IsFullwindow = !ImwGame::IsFullwindow;
    }
    if (KeyBindings::KEY_SCREENSHOT.isPressed())
    {
        Log::info("Not supported screenshot");
        //Controls::saveScreenshot();
    }
    if (KeyBindings::KEY_FULLSCREEN.isPressed())
    {
        Window::ToggleFullscreen();
    }
    if (KeyBindings::KEY_COMMAND.isPressed())
    {
        Settings::w_Console_FocusInput = true;
    }
    if (KeyBindings::KEY_DEBUG_INFO.isPressed())
    {
        bool dbg = !ImwGame::Gizmos::TextInfo;
        ImwGame::Gizmos::TextInfo =
        ImwGame::Gizmos::WorldBasis =
        ImwGame::Gizmos::ViewBasis =
        ImwGame::Gizmos::ViewGizmo = dbg;
    }

    // Follow are Gameplay keys.
    if (!Ethertia::isIngame() || !Ethertia::GetWorld())
        return;

//    HitCursor& cur = Ethertia::getHitCursor();
//    EntityPlayer& player = *Ethertia::getPlayer();
//    bool isCreativeMode = player.getGamemode() == Gamemode::CREATIVE;
//    bool isSurvivalMode = player.getGamemode() == Gamemode::SURVIVAL;
//
//    const Material* hitCellMtl = nullptr;
//    if (cur.cell && cur.cell->mtl) {
//        hitCellMtl = cur.cell->mtl;
//    }
//
//    // Pick Terrain
//    if (hitCellMtl && KeyBindings::KEY_G_PICK.isPressed() && isCreativeMode)
//    {
//        ItemStack stack(cur.cell->mtl->m_MaterialItem, 1);
//        stack.moveTo(player.getHoldingItem());
//    }
//
//    // Break Terrain
//    World* world = Ethertia::GetWorld();
//    if (hitCellMtl && KeyBindings::KEY_G_ATTACK.isKeyDown())
//    {
//        if (isSurvivalMode)
//        {
//            cur.cell_breaking_time += Ethertia::getDelta();// * Controls::gDigSpeedMultiplier;
//
//            float fullDigTime = cur.cell->mtl->m_Hardness;
//            Dbg::dbg_CurrCellBreakingFullTime = fullDigTime;
//            if (cur.cell_breaking_time >= fullDigTime)
//            {
//                // Do Dig
//                const Material* mtl = cur.cell->mtl;
//                glm::vec3 pos_drop = cur.position + cur.normal * 0.2f; // +norm prevents fall down
//
//                if (mtl == Materials::LEAVES)
//                {
//                    int rnd = Timer::timestampMillis();
//                    if (rnd % 2 == 0) world->dropItem(pos_drop, ItemStack(Items::STICK, 1));
//                    if (rnd % 4 == 0) world->dropItem(pos_drop, ItemStack(Items::APPLE, 1));
//                    if (rnd % 4 == 0) world->dropItem(pos_drop, ItemStack(Items::LIME, 1));
//                }
//                else
//                {
//                    world->dropItem(pos_drop, ItemStack(mtl->m_MaterialItem, 1));
//                }
//
//                cur.cell->set_nil();
//                world->invalidateCellFp(cur.cell_position, 3);
//                world->requestRemodel(cur.cell_position);
//                cur.cell = nullptr;
//
//
////            if (!Ethertia::getPlayer()->getHoldingItem().empty())
////            {
////                glm::vec3 cp = cur.cell_position;
////                float dig_size = cur.brushSize;
////                AABB::forCube(dig_size, [world, cp, dig_size](glm::vec3 rp)
////                {
////                    Cell& c = world->getCell(cp+rp);
////
////                    float f = std::max(0.0f, dig_size - glm::length(rp + glm::vec3(0.5f)));
////
////                    c.density -= f;
//////                    if (c.density < 0) { c.mtl = 0; }
////
////                    world->requestRemodel(cp+rp);
////                });
////            }
//
//
//                // clear hit-cell state.
//                cur.cell_breaking_time = 0;
//            }
//        }
//
//        float CurrTime = Ethertia::getPreciseTime();
//        static float s_LastTimeBreakTerrain = 0;
//        if (isCreativeMode && (s_LastTimeBreakTerrain + Settings::gInterval_BreakingTerrain_CreativeMode) <= CurrTime)
//        {
//            s_LastTimeBreakTerrain = CurrTime;
//
//            // Break Instant and No Drops.
//            glm::vec3 p = cur.position;
//            float n = cur.brushSize;
//            for (int dx = std::floor(-n); dx <= std::ceil(n); ++dx) {
//                for (int dz = std::floor(-n); dz <= std::ceil(n); ++dz) {
//                    for (int dy = std::floor(-n); dy <= std::ceil(n); ++dy) {
//                        glm::vec3 d(dx, dy, dz);
//
//                        Cell& b = world->getCell(p + d);
//                        float f = n - glm::length(d);
//
//                        // b.id = placingBlock;
//                        b.density = b.density - Mth::max(0.0f, f);
//                        if (f > 0 && b.density < 0 //&& b.id == Materials::LEAVES
//                                ) {
//                            b.mtl = 0;
//                        }
//                        world->requestRemodel(p+d);
//                    }
//                }
//            }
//        }
//
//    } else {
//        cur.cell_breaking_time = 0;
//    }
//
//
//    // Use Item (could Place Terrain)
//    if (KeyBindings::KEY_G_USE.isPressed())
//    {
//        ItemStack& stack = player.getHoldingItem();
//
//        if (!stack.empty())
//        {
//            for (ItemComponent* comp : stack.item()->m_Components)
//            {
//                comp->onUse();
//            }
//            --stack.m_Amount;
//        }
//
//        // Right click two object and link them
//        if (cur.hitEntity && !cur.hitTerrain)
//        {
//            static Entity* firstEntity = nullptr;
//            static Entity* secondEntity = nullptr;
//
//            if (firstEntity)
//            {
//                secondEntity = cur.hitEntity;
//
//                btTransform frameInA, frameInB;
//                frameInA = firstEntity->m_Rigidbody->getWorldTransform();
//                frameInB = secondEntity->m_Rigidbody->getWorldTransform();
//
//                btTransform relTrans = frameInA.inverseTimes(frameInB);
//
//                auto * fixedConstraint = new btFixedConstraint(*firstEntity->m_Rigidbody, *secondEntity->m_Rigidbody, relTrans, btTransform::getIdentity());
//                world->m_DynamicsWorld->addConstraint(fixedConstraint, true);
//
//                firstEntity = nullptr;
//                secondEntity = nullptr;
//            }
//            else
//            {
//                firstEntity = cur.hitEntity;
//            }
//        }
//    }
//
//
//    // Drop Item
//    if (KeyBindings::KEY_G_DROPITEM.isPressed())
//    {
//        ItemStack& stack = player.getHoldingItem();
//        if (!stack.empty())
//        {
//            ItemStack drop;
//
//            bool dropAll = Ethertia::getWindow().isCtrlKeyDown();
//            stack.moveTo(drop, dropAll ? stack.amount() : 1);
//
//            Ethertia::GetWorld()->dropItem(player.position(), drop,player.getViewDirection() * 3.0f);
//        }
//    }
//
//
//    // Hotbar Scrolling / Key Switch.
//    if (KeyBindings::KEY_G_HOTBAR1.isPressed()) player.m_HotbarSlot = 0;
//    if (KeyBindings::KEY_G_HOTBAR2.isPressed()) player.m_HotbarSlot = 1;
//    if (KeyBindings::KEY_G_HOTBAR3.isPressed()) player.m_HotbarSlot = 2;
//    if (KeyBindings::KEY_G_HOTBAR4.isPressed()) player.m_HotbarSlot = 3;
//    if (KeyBindings::KEY_G_HOTBAR5.isPressed()) player.m_HotbarSlot = 4;
//
//
//
//    static Entity*      s_HoldingEntity = nullptr;  // use this persistent instead of get hitEntity every time, prevents lost of tracking
//    static glm::vec3    s_HoldingEntityRelPos;  // precisely hold the hitPoint & hitDistance
//    static float        s_HoldingEntityDist;
//
//    //Constrain the object directly in front of the camera at all times
//    if (KeyBindings::KEY_G_HOLD.isPressed())
//    {
//        if (s_HoldingEntity) {
//            s_HoldingEntity = nullptr;
//        } else {
//            if (cur.hitEntity && !cur.hitTerrain) {
//                s_HoldingEntity = cur.hitEntity;
//                s_HoldingEntityRelPos = cur.position - cur.hitEntity->position();
//                s_HoldingEntityDist = cur.length;
//            }
//        }
//    }
//    // (tmp) update for HoldingEntity above
//    if (s_HoldingEntity)
//    {
//        btVector3 cameraPosition = Mth::btVec3(Ethertia::getCamera().position);
//        btVector3 cameraForward = Mth::btVec3(Ethertia::getCamera().direction);
//        btVector3 objectPosition = cameraPosition + cameraForward * s_HoldingEntityDist - Mth::btVec3(s_HoldingEntityRelPos);
//        s_HoldingEntity->m_Rigidbody->getWorldTransform().setOrigin(objectPosition);
//        // seems this have not rotate 'correctly.?' if you want rotation as cam-dir, maybe can use Quaternion e.g. setBasis(to_mat3(to_quat(camDir)))
////            target_entity->m_Rigidbody->getWorldTransform().setBasis(btMatrix3x3(cameraForward.getX(), 0.0f, 0.0f,
////                                                             0.0f, cameraForward.getY(), 0.0f,
////                                                             0.0f, 0.0f, cameraForward.getZ()));
//
////        Camera& cam = Ethertia::getCamera();
////        glm::vec3 force_pos = cam.position + cam.direction * s_HoldingEntityDist - s_HoldingEntityRelPos;
////        s_HoldingEntity->m_Rigidbody->getWorldTransform().setOrigin(Mth::btVec3(force_pos));
//    }
//
////    if (KeyBindings::KEY_G_SITTING.isPressed())
////    {
////        EntityDrivingSeat* entityDrivingSeat = dynamic_cast<EntityDrivingSeat*>(Ethertia::getHitCursor().hitEntity);
////        if (entityDrivingSeat)
////        {
////            entityDrivingSeat->setDriver(Ethertia::getPlayer());
////        }
////    }
//
}


static void _HitRaycast()
{
    if (!Ethertia::GetHitResult().enabled)
        return;

    Camera& cam = Ethertia::GetCamera();
    World* world = Ethertia::GetWorld();

    glm::vec3 origin = cam.position;
    glm::vec3 dir = cam.direction();

    if (!Ethertia::isIngame())
    {
        // dir = cam.ComputeRay();
    }


    glm::vec3 pos;
    glm::vec3 norm;
    PxShape* shape;
    PxRigidActor* actor;

    HitResult& hit = Ethertia::GetHitResult();

    hit = {};  // reset

    // todo: bugfix: Ignore Player Self.
    if (world->Raycast(origin, dir, 100.0f, pos, norm, &shape, &actor))
    {
        hit.hit = true;
        hit.position = pos;
        hit.normal = norm;
        hit.distance = glm::length(origin - pos);  // raycast actually provided distance value.
        
        Entity entity = { (entt::entity)(intptr_t)actor->userData, world };
        
        if (world->registry().valid(entity))
        {
            hit.entity = entity;

            hit.hitVoxel = entity.HasComponent<TagComponent>();
        }



        // check is valid

        //hit.hitVoxel = hit.entity.HasComponent<ChunkComponent>();
    }

}

void handleHitCursor()
{
//    World* world = Ethertia::GetWorld();
//    Camera& camera = Ethertia::getCamera();
//
//    HitCursor& cur = Ethertia::getHitCursor();
//    if (!(cur.keepTracking && world))
//        return;
//
//    glm::vec3 p, n;
//    glm::vec3 _p_beg = camera.position;
//
//    btCollisionObject* obj = nullptr;
//    cur.hit = world->raycast(_p_beg, _p_beg + camera.direction * 100.0f, p, n, &obj);
//
//    if (!cur.hit) {
//        cur.reset();
//        return;
//    }
//
//    cur.position = p;
//    cur.normal = n;
//    cur.hitEntity = (Entity*)obj->getUserPointer();
//    cur.length = glm::length(p - _p_beg);
//
//    // Hit Terrain Cell.
//    cur.hitTerrain = dynamic_cast<EntityMesh*>(cur.hitEntity);
//    if (cur.hitTerrain)
//    {
//        glm::vec3 cp_base = glm::floor(p - n*0.01f);  // cell_pos. try get full block, shrink: p-n*Epsilon
//        Cell* hitCell = &world->getCell(cp_base);
//        cur.cell_position = cp_base;
//
//        // check smooth terrain. for 8 corners, sel a valid cell that closest to the hit point.
//        if (!hitCell->mtl || hitCell->isSmoothMtl()) {  // nil or smooth-mtl.
//            glm::vec3 sel_p{INFINITY};
//            float min_dist = INFINITY;
//            for (int i = 0; i < 8; ++i) {
//                glm::vec3 cp = cp_base + SurfaceNetsMeshGen::VERT[i];  // corners. cell_pos.
//                const Cell& c = world->getCell(cp);
//                float dist = glm::length2(cp-p);
//                if (c.mtl && dist < min_dist) {
//                    min_dist = dist;
//                    sel_p = cp;
//                }
//            }
//            if (sel_p.x != INFINITY) {
//                cur.cell_position = sel_p;
//                hitCell = &world->getCell(sel_p);
//            }
//        }
//
//        if (hitCell->mtl == nullptr || hitCell != cur.cell) {
//            cur.cell_breaking_time = 0;  // reset breaking_time. hitting target changed.
//        }
//        if (hitCell->mtl == nullptr) {
//            hitCell = nullptr;
//        }
//
//        cur.cell = hitCell;
//    }
}


















glm::mat4 _MatView_MoveRotate(glm::mat4 view, glm::vec3 moveDelta, float yawDelta, float pitchDelta, float len = 0.1f, glm::vec3 moveAbsDelta = { 0, 0, 0 }, glm::vec3* out_Eye = nullptr)
{
    glm::vec3 right = glm::vec3(view[0][0], view[1][0], view[2][0]);
    glm::vec3 up = glm::vec3(view[0][1], view[1][1], view[2][1]);
    glm::vec3 forward = glm::vec3(view[0][2], view[1][2], view[2][2]);
    //right.y = 0;

    //view = glm::translate(view, moveDelta.x * right);
    //view = glm::translate(view, moveDelta.y * up); 
    //view = glm::translate(view, moveDelta.z * forward);

    glm::mat4 invView = glm::inverse(view);

    glm::vec3 eye = glm::vec3(invView[3]);
    glm::vec3 pivot = eye - forward * len;

    glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(yawDelta), { 0, 1, 0 });
    glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(pitchDelta), right);
    glm::vec3 newForward = glm::normalize(glm::mat3(yawMatrix) * glm::mat3(pitchMatrix) * forward);

    // newForward shouldn't parallels with Y, or lookAt will produce NaN
    {
        static const float rE = 0.1f;
        vec2 xz{ newForward.x, newForward.z };
        float t = glm::length(xz);
        if (t < rE)
        {
            xz *= rE / t;
            newForward.x = xz.x;
            newForward.z = xz.y;
        }
    }


    glm::vec3 newEye = pivot + newForward * len;

    moveDelta = glm::mat3(invView) * moveDelta;
    pivot += moveDelta + moveAbsDelta;
    newEye += moveDelta + moveAbsDelta;

    // output view position. since get pos from ViewMatrix is expensive (mat4 inverse)
    if (out_Eye) *out_Eye = newEye;

    auto m = glm::lookAt(newEye, pivot, { 0, 1, 0 });

    ET_ASSERT(std::isfinite(m[0][0]) && std::isfinite(m[0][1]) && std::isfinite(m[1][0]));

    return m;
}

static void _EditorMoveCamera()
{
    float dYaw = 0;
    float dPitch = 0;
    glm::vec3 move{};
    glm::vec3 moveaa{};  // axis align, for mc-like shift space - down up
    float len = 0.1f;

    vec2 MouseDelta = Window::MouseDelta();
    float  MouseWheel = Window::MouseWheelSum();

    float rotateSpeed = 0.2;

    bool keyAltDown = Window::isAltKeyDown();
    bool keyCtrlDown = Window::isCtrlKeyDown();
    bool dragLMB = Window::isMouseLeftDown();
    bool dragMMB = Window::isMouseMiddleDown();
    bool dragRMB = Window::isMouseRightDown();

    if (dragLMB || dragRMB || dragMMB)
    {
        if ((dragLMB && keyAltDown) || (dragRMB && dragLMB))
        {
            // Pin XZ
            move.z += -MouseDelta.y;
            move.x += -MouseDelta.x;
        }
        else if (keyAltDown && (dragMMB || dragLMB))
        {
            // Pivot Rotate
            dYaw += -MouseDelta.x * rotateSpeed;
            dPitch += -MouseDelta.y * rotateSpeed;

            auto& hit = Ethertia::GetHitResult();
            len = hit ? hit.distance : 32.0f;
        }
        else if (dragMMB)
        {
            // Pin XY
            move.x += -MouseDelta.x;
            move.y += MouseDelta.y;
        }
        else if (dragRMB)
        {
            // FPS Rotate
            dYaw += -MouseDelta.x * rotateSpeed;
            dPitch += -MouseDelta.y * rotateSpeed;
        }

        auto& io = ImGui::GetIO();
        if (io.KeysDown[ImGuiKey_W]) move.z -= 1;
        if (io.KeysDown[ImGuiKey_S]) move.z += 1;
        if (io.KeysDown[ImGuiKey_A]) move.x -= 1;
        if (io.KeysDown[ImGuiKey_D]) move.x += 1;
        if (io.KeysDown[ImGuiKey_Q]) move.y -= 1;
        if (io.KeysDown[ImGuiKey_E]) move.y += 1;
        if (keyCtrlDown && !dragLMB) moveaa.y -= 1;
        if (io.KeysDown[ImGuiKey_Space]) moveaa.y += 1;


        Controls::EditorCameraMoveSpeed += MouseWheel * 2;
        Controls::EditorCameraMoveSpeed = std::max(0.0f, Controls::EditorCameraMoveSpeed);

        float spd = Ethertia::GetDelta() * Controls::EditorCameraMoveSpeed;

        move *= spd;
        moveaa *= spd;

    }
    else if (MouseWheel)  // should restrict to Only WindowHovered
    {
        // Zoom Z
        move.z += -MouseWheel * Controls::EditorCameraMoveSpeed;
    }



    if (dYaw || dPitch || !Math::IsZero(move) || !Math::IsZero(moveaa))
    {
        Camera& cam = Ethertia::GetCamera();
        cam.matView = _MatView_MoveRotate(cam.matView, move, dYaw, dPitch, len, moveaa, &cam.position);
    }
}



void Controls::HandleInput()
{
    OPTICK_EVENT();

    if (Window::isCloseRequested())
        Ethertia::Shutdown();

    _HandleKeyPress();

    Camera& cam = Ethertia::GetCamera();
    World* world = Ethertia::GetWorld();


    {
        static int _SumFPS = 0;
        ++_SumFPS;

        double curr_sec = Window::PreciseTime();
        static double _LastSec{ curr_sec };
        if (curr_sec - _LastSec > 1.0f)
        {
            Controls::DbgAvgFPS = _SumFPS;
            _SumFPS = 0;
            _LastSec = curr_sec;

            //Log::info("avg FPS: {}, {}ms", Controls::DbgAvgFPS, 1.0 / Controls::DbgAvgFPS * 1000);
        }
    }

    Window::SetMouseGrabbed(Ethertia::isIngame());
    Window::SetStickyKeys(!Ethertia::isIngame());




    if (!Ethertia::isIngame() && (ImwGame::IsGameWindowHoveredOrFocused))
    {
        _EditorMoveCamera();
    }

    //camera.position = Ethertia::getPlayer()->position();

    if (world)
    {
        if (Ethertia::isIngame())
        {
            float dt = Ethertia::GetDelta();

            cam.updateMovement(dt, Window::MouseDelta().x, Window::MouseDelta().y, Window::isKeyDown(GLFW_KEY_Z));

            PxController* cct = World::dbg_CCT;

            static vec3 _GravityVelocity{};  // Low Damping
            static vec3 _MovementVelocity{};  // High Damping
            static bool _OnGround = false;

            vec3 disp{};
            if (Window::isKeyDown(GLFW_KEY_W)) disp.z -= 1;
            if (Window::isKeyDown(GLFW_KEY_S)) disp.z += 1;
            if (Window::isKeyDown(GLFW_KEY_A)) disp.x -= 1;
            if (Window::isKeyDown(GLFW_KEY_D)) disp.x += 1;

            if (Window::isShiftKeyDown())
            {
                disp *= 1.8f;
            }

            if (Window::isCtrlKeyDown()) disp.y -= 1;

            if (Window::isKeyDown(GLFW_KEY_SPACE) && _OnGround)  //  jump
            {
                disp.y += 16;
                Log::info("Jmp");
            }

            float yaw = cam.eulerAngles.y;
            disp = vec3(glm::rotate(mat4(1.0f), yaw, vec3(0, 1, 0)) * vec4(disp, 1.0f));


            if (_OnGround) {
                _MovementVelocity *= std::pow(0.002f, dt);  // High Damping due MoveDesire / Friction.
            } else {
                _MovementVelocity *= std::pow(0.95f, dt);  // Apply Damping
                disp *= 0.05f;  // InAir, Less Move
            }
            _MovementVelocity += disp;


            _GravityVelocity += glm::vec3(0, -10.0f, 0) * dt;  // Apply Gravity
            _GravityVelocity *= std::pow(0.95f, dt);  // Apply Damping
            if (_OnGround) {
                //_GravityVelocity = {0, 0,0};
            }


            PxControllerCollisionFlags collisionFlags = cct->move(stdx::cast<PxVec3>(_MovementVelocity * dt + _GravityVelocity * dt), 0.01f, dt, PxControllerFilters());
            _OnGround = collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN;


            PxExtendedVec3 p = cct->getFootPosition();
            cam.position = {p.x, p.y + 1.8f, p.z};

        }
        else
        {
            World::dbg_CCT->setFootPosition(PxExtendedVec3{ cam.position.x, cam.position.y-1.8f, cam.position.z });
        }
    }
    

    cam.UpdateMatrix(Ethertia::GetViewport().AspectRatio(), Ethertia::isIngame());

    if (world)
    {
        // After Camera Update.
        _HitRaycast();
    }




    //// Player Move.
    //
    //if (KeyBindings::KEY_G_SPRINT.isKeyDown()) {
    //    player->setSprint(true);
    //} else if (!KeyBindings::KEY_G_FORWARD.isKeyDown()) {
    //    player->setSprint(false);
    //}

//    EntityHelicopter* helicopter = dynamic_cast<EntityHelicopter*>(Imgui::g_InspEntity);
//    if (!helicopter) {
//        player->move(KeyBindings::KEY_G_JUMP.isKeyDown(), KeyBindings::KEY_G_SNEAK.isKeyDown(),
//                     KeyBindings::KEY_G_FORWARD.isKeyDown(), KeyBindings::KEY_G_BACK.isKeyDown(),
//                     KeyBindings::KEY_G_LEFT.isKeyDown(), KeyBindings::KEY_G_RIGHT.isKeyDown());
//    }
//    else
//    {
//        assert(false);
////        helicopter->move(window.isKeyDown(GLFW_KEY_SPACE), window.isKeyDown(GLFW_KEY_LEFT_SHIFT),
////                                 window.isKeyDown(GLFW_KEY_W), window.isKeyDown(GLFW_KEY_S),
////                                 window.isKeyDown(GLFW_KEY_A), window.isKeyDown(GLFW_KEY_D),
////                                 window.isKeyDown(GLFW_KEY_LEFT_BRACKET), window.isKeyDown(GLFW_KEY_RIGHT_BRACKET));
//    }


    // Camera Move
//    static SmoothValue smFov;
//    smFov.update(dt);
//    if (window.isKeyDown(GLFW_KEY_X)) {
//        smFov.target += window.getMouseY() / 800.0f;
//    }
//    RenderEngine::fov += smFov.delta;



}


//void Controls::saveScreenshot()
//{
//    assert("NotSupported");
//    BitmapImage* img = Ethertia::getWindow().screenshot();
//
//    std::string path = Strings::fmt("./screenshots/{}_{}.png", Strings::time_fmt(-1, "%Y-%m-%d_%H.%M.%S"), (Mth::frac(Ethertia::getPreciseTime())*1000.0f));
//    if (Loader::fileExists(path))
//        throw std::logic_error("File already existed.");
//
//    Log::info("Screenshot saving to '{}'.\1", path);
//    Ethertia::notifyMessage(Strings::fmt("Saved screenshot to '{}'.", path));
//
//    Ethertia::getAsyncScheduler().addTask([img, path]() {
//        BENCHMARK_TIMER;
//
//        // vertical-flip image back to normal. due to GL feature.
//        BitmapImage fine_img(img->width(), img->height());
//        img->getVerticalFlippedPixels(fine_img.pixels());
//
//        Loader::savePNG(path, fine_img);
//        delete img;
//    });
//}