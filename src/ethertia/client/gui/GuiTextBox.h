//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUITEXTBOX_H
#define ETHERTIA_GUITEXTBOX_H

#include <utility>

#include "Gui.h"
#include "GuiButton.h"

#include <ethertia/client/Ethertia.h>
#include <ethertia/client/render/renderer/gui/FontRenderer.h>

class GuiTextBox : public Gui
{
    std::string text;

    float textX = 0;
    float textY = 0;
    float textHeight = 16;

    int cursorPosition = 0;

    int selectionBegin = 0;
    int selectionEnd   = 0;

    bool singleline = false;

    float _timeLastCursorModify;  // for rendering. after cursor move, it should show a while.

public:
    GuiTextBox(std::string text) : text(std::move(text)), Gui(0, 0, 100, 20) {

        EventBus::EVENT_BUS.listen([&](KeyboardEvent* e) {
            if (!isVisible()) return;

            if (e->isPressed() && isFocused()) {
                int key = e->getKey();

                if (key == GLFW_KEY_LEFT) {
                    if (hasSelection()) {
                        setCursorPosition(getSelectionMin());
                        clearSelection();
                    }
                    incCursorPosition(-1);
                } else if (key == GLFW_KEY_RIGHT) {
                    if (hasSelection()) {
                        setCursorPosition(getSelectionMax());
                        clearSelection();
                    }
                    incCursorPosition(1);
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
                    selectionBegin = 0;
                    selectionEnd = this->text.length();
                    setCursorPosition(selectionEnd);
                }
            }
        });

        EventBus::EVENT_BUS.listen([&](CharInputEvent* e) {
            if (!isVisible()) return;
            if (isFocused()) {
                insertText(std::string(1, e->getChar()));
            }
        });

        EventBus::EVENT_BUS.listen([&](MouseButtonEvent* e) {
            if (!isVisible()) return;
            if (e->isPressed()) {
                int idx = Ethertia::getRenderEngine()->fontRenderer->textIdx(this->text, textHeight, Gui::cursorX()-textX-getX(), Gui::cursorY()-textY-getY());
                selectionBegin = idx;
                selectionEnd = idx;
            }
        });
    }

    void onDraw() override
    {
        float x = getX(), y = getY(), w = getWidth(), h = getHeight();

        GuiButton::drawButtonBackground(this);

        FontRenderer* fr = Ethertia::getRenderEngine()->fontRenderer;

        // Selection
        if (isPressed()) {
            int idx = fr->textIdx(text, textHeight, Gui::cursorX()-textX-x, Gui::cursorY()-textY-y);
            setCursorPosition(idx);

            selectionEnd = idx;
        }
        for (int i = getSelectionMin(); i < getSelectionMax(); ++i) {
            glm::vec2 p = fr->textPos(text, textHeight, i);
            Gui::drawRect(x+textX + p.x, y+textY + p.y, fr->charFullWidth(text[i], textHeight), textHeight, Colors::WHITE30);
        }

        Gui::drawString(x, y, text, Colors::WHITE, textHeight);

        // Cursor
        if (isFocused() && (Mth::mod(Ethertia::getPreciseTime(), 1.0f) < 0.5f || Ethertia::getPreciseTime() - _timeLastCursorModify < 0.5f)) {
            glm::vec2 p = calcTextPos(getCursorPosition());

            Gui::drawRect(x+textX + p.x, y+textY + p.y, 2, textHeight, Colors::WHITE);
        }

        Gui::onDraw();
    }

    void setText(const std::string& t) {
        if (t == text) return;
        text = t;
    }

    [[nodiscard]] const std::string& getText() const {
        return text;
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
        return Mth::min(Mth::max(selectionBegin, selectionEnd), (int)text.length());
    }

    int getCursorPosition() {
        return Mth::clamp(cursorPosition, 0, (int)text.length());
    }
    void setCursorPosition(int i) {
        cursorPosition = i;

        _timeLastCursorModify = Ethertia::getPreciseTime();
    }
    void incCursorPosition(int n) {
        setCursorPosition(getCursorPosition() + n);
    }

    glm::vec2 calcTextPos(int i) {
        return Ethertia::getRenderEngine()->fontRenderer->textPos(text, textHeight, i);
    }

    class OnTextChanged {};

    void insertText(const std::string& t) {
        if (hasSelection()) {
            int n = getSelectionMax() - getSelectionMin();
            text.erase(getSelectionMin(), n);
            setCursorPosition(getSelectionMin());
            clearSelection();
        }

        text.insert(getCursorPosition(), t);
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
        return text.substr(getSelectionMin(), getSelectionMax()-getSelectionMin());
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
