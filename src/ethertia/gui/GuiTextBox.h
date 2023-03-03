//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUITEXTBOX_H
#define ETHERTIA_GUITEXTBOX_H

#include <utility>

#include "Gui.h"
#include "GuiButton.h"

#include <ethertia/Ethertia.h>
#include <ethertia/render/gui/FontRenderer.h>

class GuiTextBox : public Gui
{
    std::string m_Text;

    float textHeight = 16;

    int m_CursorIndex = 0;

    int selectionBegin = 0;
    int selectionEnd   = 0;

    bool singleline = false;

    float _timeLastCursorModify;  // for rendering. after cursor move, it should show a while.

public:

    float m_TextX = 0;
    float m_TextY = 0;

    std::string m_Caption = "";
    std::string m_Hint = "";

    GuiTextBox(std::string text) : m_Text(std::move(text)), Gui(0, 0, 200, 24) {
        m_TextX = 4;
        m_TextY = 4;
    }

    void implDraw() override
    {
        float x = getX(), y = getY(), w = getWidth(), h = getHeight();
        if (!m_Caption.empty()) {
            drawString(x, y-14, m_Caption, Colors::GRAY, 12);
        }

        GuiButton::drawButtonBackground(this);

        drawRect({
           .xywh = xywh(),
           .color = Colors::BLACK20
        });
        drawRect({
            .xywh = xywh(),
            .color = Colors::WHITE20,
            .border = 2
        });

        FontRenderer* fr = Ethertia::getRenderEngine()->fontRenderer;

        // Selection
        if (isPressed()) {
            int idx = fr->textIdx(m_Text, textHeight, Gui::cursorX() - m_TextX - x, Gui::cursorY() - m_TextY - y);
            setCursorPosition(idx);

            selectionEnd = idx;
        }
        for (int i = getSelectionMin(); i < getSelectionMax(); ++i) {
            glm::vec2 p = fr->textPos(m_Text, textHeight, i);
            Gui::drawRect(x + m_TextX + p.x, y + m_TextY + p.y, fr->charFullWidth(m_Text[i], textHeight), textHeight, Colors::WHITE30);
        }

        Gui::drawString(x+m_TextX, y+m_TextY, m_Text, Colors::WHITE, textHeight);

        if (m_Text.empty() && !m_Hint.empty()) {
            Gui::drawString(x+m_TextX, y+m_TextY, m_Hint, Colors::GRAY, textHeight);
        }

        // Cursor
        if (isFocused() && (Mth::mod(Ethertia::getPreciseTime(), 1.0f) < 0.5f || Ethertia::getPreciseTime() - _timeLastCursorModify < 0.5f)) {
            glm::vec2 p = calcTextPos(getCursorPosition());

            Gui::drawRect(p.x, p.y, 2, textHeight, Colors::WHITE);
        }

    }

    void onKeyboard(int key, bool pressed) override {

        if (pressed && isFocused()) {

            if (key == GLFW_KEY_LEFT) {
                if (hasSelection()) {
                    setCursorPosition(getSelectionMin());
                } else {
                    incCursorPosition(-1);
                }
            } else if (key == GLFW_KEY_RIGHT) {
                if (hasSelection()) {
                    setCursorPosition(getSelectionMax());
                } else {
                    incCursorPosition(1);
                }
            } else if (key == GLFW_KEY_BACKSPACE) {
                backspaceText();
            } else if (key == GLFW_KEY_ENTER) {
                if (singleline) {
                    fireEvent(OnReturn());
                } else {
                    insertText("\n");
                }
            } else if (key == GLFW_KEY_V && Ethertia::getWindow()->isCtrlKeyDown()) {
                insertText(Ethertia::getWindow()->getClipboard());
            } else if (key == GLFW_KEY_C && Ethertia::getWindow()->isCtrlKeyDown()) {
                Ethertia::getWindow()->setClipboard(getSelectedText().c_str());
            } else if (key == GLFW_KEY_X && Ethertia::getWindow()->isCtrlKeyDown()) {
                Ethertia::getWindow()->setClipboard(getSelectedText().c_str());
                insertText("");
            } else if (key == GLFW_KEY_A && Ethertia::getWindow()->isCtrlKeyDown()) {
                setSelection(0, m_Text.length());
            }
        }

    }

    void onCharInput(int ch) override {

        if (isFocused()) {
            // not support chinese characters yet.
            insertText(std::string(1, ch));
        }

    }

    void onMouseButton(int button, bool pressed) override {

        Log::info("MB");

        if (pressed) {
            int idx = Ethertia::getRenderEngine()->fontRenderer->textIdx(m_Text, textHeight, Gui::cursorX() - m_TextX - getX(), Gui::cursorY() - m_TextY - getY());
//            selectionBegin = idx;
//            selectionEnd = idx;
            setCursorPosition(idx);
        }

    }

    void setText(const std::string& t) {
        if (t == m_Text) return;
        m_Text = t;

        setCursorPosition(m_Text.size());
    }

    [[nodiscard]] const std::string& getText() const {
        return m_Text;
    }

    bool hasSelection() {
        return selectionBegin != selectionEnd;
    }
    void clearSelection() {
        selectionBegin = 0;
        selectionEnd = 0;
    }
    int getSelectionMin() {
        return Mth::max(Mth::min(selectionBegin, selectionEnd), 0);
    }
    int getSelectionMax() {
        return Mth::min(Mth::max(selectionBegin, selectionEnd), (int)m_Text.length());
    }
    void setSelection(int min, int max) {
        selectionBegin = min;
        selectionEnd = max;
        setCursorPosition(max);
    }

    int getCursorPosition() {
        return Mth::clamp(m_CursorIndex, 0, (int)m_Text.length());
    }
    void setCursorPosition(int i) {
        m_CursorIndex = i;
        selectionBegin = i;
        selectionEnd = i;

        _timeLastCursorModify = Ethertia::getPreciseTime();
    }
    void incCursorPosition(int n) {
        setCursorPosition(getCursorPosition() + n);
    }

    glm::vec2 calcTextPos(int i) {
        glm::vec2 p = Ethertia::getRenderEngine()->fontRenderer->textPos(m_Text, textHeight, i);
        return {
                getX() + m_TextX + p.x,
                getY() + m_TextY + p.y
        };
    }

    class OnTextChanged {};

    void insertText(const std::string& t) {
        if (hasSelection()) {
            int n = getSelectionMax() - getSelectionMin();
            m_Text.erase(getSelectionMin(), n);
            setCursorPosition(getSelectionMin());
            clearSelection();
        }

        m_Text.insert(getCursorPosition(), t);
        incCursorPosition(t.length());

        fireEvent(OnTextChanged());
    }
    void backspaceText() {
        if (!hasSelection()) {
            selectionEnd = getCursorPosition();
            selectionBegin = selectionEnd - 1;
        }

        insertText("");
    }

    std::string getSelectedText() {
        return m_Text.substr(getSelectionMin(), getSelectionMax()-getSelectionMin());
    }

    float getTextHeight() {
        return textHeight;
    }

    void addOnTextChangedListener(const std::function<void(OnTextChanged*)>& lsr) {
        eventbus.listen(lsr);
    }

    void setSingleLine(bool i) {
        singleline = i;
    }

    class OnReturn {};

    void addOnReturnListener(const std::function<void(OnReturn*)>& lsr) {

        eventbus.listen(lsr);
    }
};

#endif //ETHERTIA_GUITEXTBOX_H
