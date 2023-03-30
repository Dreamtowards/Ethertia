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


    bool isPressed()  { return ImGui::IsKeyPressed(m_Key); }  // once, from !Down to Down
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
        KEY_G_DROPITEM      {ImGuiKey_Q,            "G: Drop item"},
        KEY_G_SPRINT        {ImGuiKey_LeftCtrl,     "G: Sprint"},
        KEY_G_FORWARD       {ImGuiKey_W,            "G: Forward"},
        KEY_G_BACK          {ImGuiKey_S,            "G: Back"},
        KEY_G_LEFT          {ImGuiKey_A,            "G: Left"},
        KEY_G_RIGHT         {ImGuiKey_D,            "G: Right"},
        KEY_G_JUMP          {ImGuiKey_Space,        "G: Jump"},
        KEY_G_SNEAK         {ImGuiKey_LeftShift,    "G: Sneak"},
        KEY_G_CAM_DIST      {ImGuiKey_V,            "G: Camera Distance"},
        KEY_G_HOTBAR1       {ImGuiKey_1,            "G: Hotbar 1"},
        KEY_G_HOTBAR2       {ImGuiKey_2,            "G: Hotbar 2"},
        KEY_G_HOTBAR3       {ImGuiKey_3,            "G: Hotbar 3"},
        KEY_G_HOTBAR4       {ImGuiKey_4,            "G: Hotbar 4"},
        KEY_G_HOTBAR5       {ImGuiKey_5,            "G: Hotbar 5"}
        ;

};

#endif //ETHERTIA_KEYBINDING_H
