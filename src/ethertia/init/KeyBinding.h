//
// Created by Dreamtowards on 2023/3/29.
//

#ifndef ETHERTIA_KEYBINDING_H
#define ETHERTIA_KEYBINDING_H

class KeyBinding
{
public:
    inline static Registry<KeyBinding> REGISTRY;

    ImGuiKey m_Key;
    const ImGuiKey m_DefaultKey;
    const char* m_Name;

    KeyBinding(const ImGuiKey defkay, const char* name) : m_DefaultKey(defkay), m_Key(defkay), m_Name(name)
    {
        REGISTRY.regist(this);
    }
DECL_RegistryId(m_Name);

    ImGuiKey key() {
        return m_Key;
    }


    bool isPressed(bool repeat=false)  { return ImGui::IsKeyPressed(m_Key, repeat); }  // once, from !Down to Down
    bool isReleased() { return ImGui::IsKeyReleased(m_Key); }
    bool isKeyDown()  { return ImGui::IsKeyDown(m_Key); }  // is hold


};

class KeyBindings
{
public:
    inline static KeyBinding
        KEY_ESC             {ImGuiKey_Escape,       "Pause/Resume"},
        KEY_FULL_VIEWPORT   {ImGuiKey_GraveAccent,  "Full Viewport"},
        KEY_SCREENSHOT      {ImGuiKey_F2,           "Screenshot"},
        KEY_DEBUG_INFO      {ImGuiKey_F3,           "Debug info"},
        KEY_FULLSCREEN      {ImGuiKey_F11,          "Fullscreen"},
        KEY_COMMAND         {ImGuiKey_Slash,        "Command"},
        //  G_ means Gameplay category here.
        KEY_G_DROPITEM      {ImGuiKey_Q,            "Drop item"},
        KEY_G_SPRINT        {ImGuiKey_LeftCtrl,     "Sprint"},
        KEY_G_FORWARD       {ImGuiKey_W,            "Forward"},
        KEY_G_BACK          {ImGuiKey_S,            "Back"},
        KEY_G_LEFT          {ImGuiKey_A,            "Left"},
        KEY_G_RIGHT         {ImGuiKey_D,            "Right"},
        KEY_G_JUMP          {ImGuiKey_Space,        "Jump"},
        KEY_G_SNEAK         {ImGuiKey_LeftShift,    "Sneak"},
        KEY_G_CAM_DIST      {ImGuiKey_V,            "Camera Distance"},
        KEY_G_HOTBAR1       {ImGuiKey_1,            "Hotbar 1"},
        KEY_G_HOTBAR2       {ImGuiKey_2,            "Hotbar 2"},
        KEY_G_HOTBAR3       {ImGuiKey_3,            "Hotbar 3"},
        KEY_G_HOTBAR4       {ImGuiKey_4,            "Hotbar 4"},
        KEY_G_HOTBAR5       {ImGuiKey_5,            "Hotbar 5"},
        KEY_G_USE           {ImGuiKey_MouseRight,   "Use/Place"},
        KEY_G_ATTACK        {ImGuiKey_MouseLeft,    "Attack/Destroy"},
        KEY_G_HOLD        {ImGuiKey_F,    "Hold"}
        ;

};

#endif //ETHERTIA_KEYBINDING_H
