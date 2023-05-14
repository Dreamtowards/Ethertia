//
// Created by Dreamtowards on 2023/5/14.
//


class ImKeymap
{
public:
    inline static float _keysize_std = 54;
    inline static ImVec2 _keysize = {_keysize_std, _keysize_std};
    inline static float _keygap = 4;

    inline static std::function<ImU32(ImGuiKey, bool hover)> g_FuncKeyColor = [](auto, auto){return -1;};

    static void RenderKey(ImVec2& p, const char* name, ImGuiKey keycode, float keySizeX = 1.0f, float sizeAddGaps = 0)
    {
        float gapAdd = sizeAddGaps * _keygap;

        ImVec2 offset = {2, 2};
        if (name[1] == '\0')
            offset = {0.5f*(_keysize.x-8), 0.5f*(_keysize.y-12)};

        ImVec2 size = ImVec2(keySizeX * _keysize.x + gapAdd, _keysize.y);
        bool hover = ImGui::IsMouseHoveringRect(p, p+size);

        ImU32 col = ImGui::GetColorU32(hover ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        if (ImGui::IsKeyDown(keycode)) {
            col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
        }
        ImU32 evalColor = g_FuncKeyColor(keycode, hover);
        if (evalColor != -1) {
            col = evalColor;
        }

        ImGui::RenderFrame(p, p+size,col, true, ImGui::GetStyle().FrameRounding);
        ImGui::RenderText(p+offset, name);

        p.x += keySizeX * _keysize.x + gapAdd + _keygap;
    }

    static void ShowKeymap()
    {
        ImVec2 begin = ImGui::GetCursorScreenPos();
        ImVec2 p = begin;

        _keysize.y = _keysize_std * 0.5f;
        RenderKey(p, "ESC", ImGuiKey_Escape); p.x += _keysize.x + _keygap;
        RenderKey(p, "F1", ImGuiKey_F1);
        RenderKey(p, "F2", ImGuiKey_F1);
        RenderKey(p, "F3", ImGuiKey_F3);
        RenderKey(p, "F4", ImGuiKey_F4); p.x += 0.5f * (_keysize.x + _keygap);
        RenderKey(p, "F5", ImGuiKey_F5);
        RenderKey(p, "F6", ImGuiKey_F6);
        RenderKey(p, "F7", ImGuiKey_F7);
        RenderKey(p, "F8", ImGuiKey_F8); p.x += 0.5f * (_keysize.x + _keygap);
        RenderKey(p, "F9", ImGuiKey_F9);
        RenderKey(p, "F10", ImGuiKey_F10);
        RenderKey(p, "F11", ImGuiKey_F11);
        RenderKey(p, "F12", ImGuiKey_F12);
        _keysize.y = _keysize_std;

#define STEP_ROW p.x = begin.x; p.y += _keysize.y + _keygap;
        STEP_ROW;
        RenderKey(p, "~\n`", ImGuiKey_GraveAccent);
        RenderKey(p, "!\n1", ImGuiKey_1);
        RenderKey(p, "@\n2", ImGuiKey_2);
        RenderKey(p, "#\n3", ImGuiKey_3);
        RenderKey(p, "$\n4", ImGuiKey_4);
        RenderKey(p, "%\n5", ImGuiKey_5);
        RenderKey(p, "^\n6", ImGuiKey_6);
        RenderKey(p, "&\n7", ImGuiKey_7);
        RenderKey(p, "*\n8", ImGuiKey_8);
        RenderKey(p, "(\n9", ImGuiKey_9);
        RenderKey(p, ")\n0", ImGuiKey_0);
        RenderKey(p, "_\n-", ImGuiKey_Minus);
        RenderKey(p, "+\n=", ImGuiKey_Equal);
        RenderKey(p, "backspace", ImGuiKey_Backspace, 2.0f, 1);

        STEP_ROW
        RenderKey(p, "tab", ImGuiKey_Tab, 1.5f);
        RenderKey(p, "Q", ImGuiKey_Q);
        RenderKey(p, "W", ImGuiKey_W);
        RenderKey(p, "E", ImGuiKey_E);
        RenderKey(p, "R", ImGuiKey_R);
        RenderKey(p, "T", ImGuiKey_T);
        RenderKey(p, "Y", ImGuiKey_Y);
        RenderKey(p, "U", ImGuiKey_U);
        RenderKey(p, "I", ImGuiKey_I);
        RenderKey(p, "O", ImGuiKey_O);
        RenderKey(p, "P", ImGuiKey_P);
        RenderKey(p, "{\n[", ImGuiKey_LeftBracket);
        RenderKey(p, "}\n]", ImGuiKey_RightBracket);
        RenderKey(p, "|\n\\", ImGuiKey_Backslash, 1.5f, 1);

        STEP_ROW;
        RenderKey(p, "caps", ImGuiKey_CapsLock, 1.75f, 1);
        RenderKey(p, "A", ImGuiKey_A);
        RenderKey(p, "S", ImGuiKey_S);
        RenderKey(p, "D", ImGuiKey_D);
        RenderKey(p, "F", ImGuiKey_F);
        RenderKey(p, "G", ImGuiKey_G);
        RenderKey(p, "H", ImGuiKey_H);
        RenderKey(p, "J", ImGuiKey_J);
        RenderKey(p, "K", ImGuiKey_K);
        RenderKey(p, "L", ImGuiKey_L);
        RenderKey(p, ":\n;", ImGuiKey_Semicolon);
        RenderKey(p, "\"\n'", ImGuiKey_Apostrophe);
        RenderKey(p, "return", ImGuiKey_Enter, 2.25f, 1);

        STEP_ROW;
        RenderKey(p, "shift", ImGuiKey_LeftShift, 2.0f, 1);
        RenderKey(p, "Z", ImGuiKey_Z);
        RenderKey(p, "X", ImGuiKey_X);
        RenderKey(p, "C", ImGuiKey_C);
        RenderKey(p, "V", ImGuiKey_V);
        RenderKey(p, "B", ImGuiKey_B);
        RenderKey(p, "N", ImGuiKey_N);
        RenderKey(p, "M", ImGuiKey_M);
        RenderKey(p, "<\n,", ImGuiKey_Comma);
        RenderKey(p, ">\n.", ImGuiKey_Period);
        RenderKey(p, "?\n/", ImGuiKey_Slash);
        RenderKey(p, "shift", ImGuiKey_RightShift, 3, 2);

        STEP_ROW;
        RenderKey(p, "fn", ImGuiKey_F);
        RenderKey(p, "ctrl", ImGuiKey_LeftCtrl);
        RenderKey(p, "alt", ImGuiKey_LeftAlt);
        RenderKey(p, "super", ImGuiKey_LeftSuper);
        RenderKey(p, "", ImGuiKey_Space, 6, 5);
        RenderKey(p, "super", ImGuiKey_RightSuper);
        RenderKey(p, "alt", ImGuiKey_RightAlt);

        float tmpY = p.y;
        _keysize.y = _keysize_std*0.5f - _keygap*0.5f;
        p.y += _keysize.y + _keygap;
        RenderKey(p, "<", ImGuiKey_LeftArrow);
        float tmpX = p.x;
        RenderKey(p, "v", ImGuiKey_DownArrow);
        RenderKey(p, ">", ImGuiKey_RightArrow);
        p = {tmpX, tmpY};
        RenderKey(p, "^", ImGuiKey_UpArrow);
        p.y += _keysize.y + _keygap;
        p.y += 4;

        STEP_ROW;
        float mbSize = _keysize.x;
        RenderKey(p, "MLB", ImGuiKey_MouseLeft);
        _keysize.x = 40;
        RenderKey(p, "MMB", ImGuiKey_MouseMiddle);
        _keysize.x = mbSize;
        RenderKey(p, "MRB", ImGuiKey_MouseRight);


        ImGui::ItemSize(p-begin + ImVec2(0, 60));
//        ImGui::SetCursorScreenPos({begin.x, p.y + 60});
    }


};
