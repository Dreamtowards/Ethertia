//
// Created by Dreamtowards on 9/8/2023.
//

#include "Imw.h"

#include <ethertia/util/Loader.h>
#include <ethertia/util/Colors.h>
#include <ethertia/util/Log.h>


void Imw::Debug::ShowDialogs(bool* _open)
{
	ImGui::Begin("Loader Dialogs", _open);

	if (ImGui::Button("ShowMessageBox (ok, info)"))
		Loader::ShowMessageBox("Title", "Msg");

	if (ImGui::Button("ShowMessageBox (okcancel warning)"))
		Loader::ShowMessageBox("Title", "Msg", "okcancel", "warning");

	if (ImGui::Button("ShowMessageBox (yesno error)"))
		Loader::ShowMessageBox("Title", "Msg", "yesno", "error");

	if (ImGui::Button("ShowMessageBox (yesnocancel question)"))
		Loader::ShowMessageBox("Title", "Msg", "yesnocancel", "question");

	if (ImGui::Button("ShowInputBox"))
	{
		const char* msg = Loader::ShowInputBox("Title", "Msg", "sth");
		if (msg) Log::info("Input: {}", msg);
	}

	if (ImGui::Button("OpenFileDialog"))
	{
		const char* s = Loader::OpenFileDialog("Title2", nullptr, {}, "Imgs Files", true);
		if (s) Log::info("Result: {}", s);
	}

	if (ImGui::Button("SaveFileDialog"))
	{
		const char* s = Loader::SaveFileDialog("Title2");
		if (s) Log::info("Result: {}", s);
	}

	if (ImGui::Button("OpenFolderDialog"))
	{
		const char* s = Loader::OpenFolderDialog("Title2", nullptr);
		if (s) Log::info("Result: {}", s);
	}

	if (ImGui::Button("OpenColorPicker"))
	{
		const char* s;
		Loader::OpenColorPicker("ColorPkr2", Colors::BLUE, nullptr, &s);
		if (s) Log::info("Result: {}", s);
	}

	ImGui::End();
}