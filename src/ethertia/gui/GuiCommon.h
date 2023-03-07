//
// Created by Dreamtowards on 2022/12/30.
//

#ifndef ETHERTIA_GUICOMMON_H
#define ETHERTIA_GUICOMMON_H

#define DECL_Inst(T) static T* Inst() { static T* INST = new T(); return INST; }

#include <ethertia/gui/GuiStack.h>
#include <ethertia/gui/GuiButton.h>
#include <ethertia/gui/GuiPopupMenu.h>
#include <ethertia/gui/GuiSlider.h>
#include <ethertia/gui/GuiCheckBox.h>
#include <ethertia/gui/GuiAlign.h>
#include <ethertia/gui/GuiText.h>
#include <ethertia/gui/GuiScrollBox.h>
#include <ethertia/gui/GuiCollection.h>
#include <ethertia/gui/GuiTextBox.h>
#include <ethertia/gui/GuiSeparator.h>

#endif //ETHERTIA_GUICOMMON_H
