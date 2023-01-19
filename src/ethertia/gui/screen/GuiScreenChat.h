//
// Created by Dreamtowards on 2022/8/22.
//

#ifndef ETHERTIA_GUISCREENCHAT_H
#define ETHERTIA_GUISCREENCHAT_H

#include <utility>

#include "../GuiCollection.h"
#include "../GuiStack.h"
#include "../GuiTextBox.h"
#include "../GuiAlign.h"

#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/command/Command.h>

class GuiScreenChat : public GuiCollection
{
public:

    inline static GuiScreenChat* INST;

    GuiTextBox* m_Input = nullptr;

    std::vector<std::string> m_History;

    int m_HistoryViewIndex = 0;
    std::string m_PresentEditing;

    bool m_ShowTabComplete = false;
    std::vector<std::string> m_TabCompletes;
    int m_TabIndex = 0;
    int m_TabPosX = 0;

    bool m_AutoCompleteCmd = true;

    GuiScreenChat() {
        setWidth(Inf);
        setHeight(Inf);



        m_Input = new GuiTextBox("");
        m_Input->setWidth(Inf);
        m_Input->setHeight(20);
        m_Input->m_TextY = (m_Input->getHeight() - m_Input->getTextHeight()) / 2;
        m_Input->addDrawBackground(Colors::BLACK80);
        m_Input->setSingleLine(true);
        m_Input->setFocused(true);
        m_Input->addOnReturnListener([this](GuiTextBox::OnReturn* e)
        {
            const std::string& line = m_Input->getText();

            Ethertia::dispatchCommand(line);

            m_History.push_back(line);

            Ethertia::getRootGUI()->removeGui(this);  // dismiss.
            m_Input->setText("");
        });
        m_Input->addOnTextChangedListener([this](GuiTextBox::OnTextChanged* e)
        {
            m_PresentEditing = m_Input->getText();
            m_HistoryViewIndex = m_History.size();

            m_TabIndex = 0;

            bool isCmd = m_Input->getText().starts_with('/');

            if (m_AutoCompleteCmd && isCmd) {
                m_ShowTabComplete = true;
                feedTabComplete();
            } else {
                m_ShowTabComplete = false;
            }

        });

        addGui(new GuiAlign(0, 1, m_Input));

    }

    void openCommandInput()
    {
        m_Input->setText("");
        m_Input->setFocused(true);

        m_HistoryViewIndex = m_History.size();
    }

    void implDraw() override
    {
        if (m_ShowTabComplete) {
            float singleTabHeight = 16;
            float tabHeight = m_TabCompletes.size() * singleTabHeight;
            float tabX = m_TabPosX;
            float tabY = m_Input->getY() - tabHeight;

            drawRect(tabX, tabY, 100, tabHeight, Colors::BLACK40);

            for (int i = 0; i < m_TabCompletes.size(); ++i) {
                Gui::drawString(tabX, tabY + i*singleTabHeight, m_TabCompletes[i],
                                i==m_TabIndex ? Colors::WHITE : Colors::GRAY_DARK);
            }
        }
    }

    void onKeyboard(int key, bool pressed) override
    {
        if (pressed) {
            if (key == GLFW_KEY_UP) {

                if (m_HistoryViewIndex == m_History.size()) {
                    m_PresentEditing = m_Input->getText();
                }

                if (m_HistoryViewIndex > 0) {
                    m_HistoryViewIndex--;

                    m_Input->setText(m_History[m_HistoryViewIndex]);
                }

            } else if (key == GLFW_KEY_DOWN) {

                if (m_HistoryViewIndex < m_History.size()) {
                    m_HistoryViewIndex++;
                }

                if (m_HistoryViewIndex == m_History.size()){
                    m_Input->setText(m_PresentEditing);
                } else {
                    m_Input->setText(m_History[m_HistoryViewIndex]);
                }

            } else if (key == GLFW_KEY_TAB) {

                if (!m_ShowTabComplete) {
                    m_TabIndex = 0;
                    m_ShowTabComplete = true;
                    feedTabComplete();
                } else if (m_TabCompletes.size() > 0) {
                    m_TabIndex++;
                    m_TabIndex = m_TabIndex % m_TabCompletes.size();
                    updateTabPos();  // for chat text-inc
                }

                if (!m_TabCompletes.empty())
                {
                    std::string& comple = m_TabCompletes[m_TabIndex];
                    const std::string& line = m_Input->getText();
                    int cur = m_Input->getCursorPosition();

                    int before = line.rfind(' ', cur);
                    if (before == -1) {
                        if (line.starts_with('/')) {
                            before = 1;
                        } else {
                            before = cur;
                        }
                    } else {
                        before += 1;
                    }

                    std::string replaced = line.substr(0, before) + comple;

                    int after = line.find(' ', cur);
                    if (after != -1)
                        replaced += line.substr(after);

                    m_Input->setText(replaced);

                }
            }
        }

        GuiCollection::onKeyboard(key, pressed);
    }

    void feedTabComplete() {
        m_TabCompletes.clear();

        int cur = m_Input->getCursorPosition();
        const std::string& line = m_Input->getText();
        if (line.starts_with('/'))
        {
            std::vector<std::string> args = Strings::splitSpaces(line.substr(0, cur));
            std::string cmd = args[0].substr(1);

            if (args.size() == 1 && !line.ends_with(' '))
            {
                for (auto& it : Command::REGISTRY) {
                    if (it.first.starts_with(cmd))
                        m_TabCompletes.push_back(it.first);
                }
            }
            else
            {

                m_TabCompletes.push_back("Sth1");
                m_TabCompletes.push_back("Sth234");
            }
        }
        else
        {
            m_TabCompletes.push_back("@TestPlayer1");
            m_TabCompletes.push_back("@TestPlayer2");
        }

        updateTabPos();
    }

    void updateTabPos() {
        int cur = m_Input->getCursorPosition();
        const std::string& line = m_Input->getText();

        int i = line.rfind(' ', cur);
        if (i == -1) {
            i = line.starts_with('/') ? 1 : cur;
        } else {
            i = i+1;
        }
        m_TabPosX = m_Input->calcTextPos(i).x;
    }

};

#endif //ETHERTIA_GUISCREENCHAT_H
