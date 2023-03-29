//
// Created by Dreamtowards on 2023/3/29.
//

#ifndef ETHERTIA_KEYBINDING_H
#define ETHERTIA_KEYBINDING_H

class KeyBinding
{
public:
    inline static Registry<KeyBinding> REGISTRY;

    const ImGuiKey m_DefaultKey;
    ImGuiKey m_Key;
    const char* m_Name;

    KeyBinding(const ImGuiKey defkay, const char* name) : m_DefaultKey(defkay), m_Key(defkay), m_Name(name)
    {
        REGISTRY.regist(this);
    }
DECL_RegistryId(m_Name);

    ImGuiKey key() {
        return m_Key;
    }


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
        KEY_COMMAND         {ImGuiKey_Slash,        "Gameplay: Command"},
        KEY_DROPITEM        {ImGuiKey_Q,            "Gameplay: Drop item"};

};

#endif //ETHERTIA_KEYBINDING_H
