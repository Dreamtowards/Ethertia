
#pragma once

#include <glm/glm.hpp>

#include <ImGuizmo.h>


class ImwGame
{
public:

	// xywz = xywh.
	inline static glm::vec4 Viewport;

	inline static bool IsFullwindow = false;

	inline static VkImageView WorldImageView = nullptr;


	inline static ImGuizmo::OPERATION	GuizmoOperation = ImGuizmo::TRANSLATE;
	inline static ImGuizmo::MODE		GuizmoMode		= ImGuizmo::WORLD;

	static void ShowGame(bool* _open);


	static void ShowWorldSettings(bool* _open);

	static void ShowWorldNew(bool* _open);

	static void ShowWorldList(bool* _open);

	static void ShowTitleScreen(bool* _open);

};