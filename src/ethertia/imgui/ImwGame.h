
#pragma once

#include <glm/glm.hpp>

#include <ImGuizmo.h>


class ImwGame
{
public:

	// xywz = xywh.
	inline static glm::vec4 Viewport;

	inline static bool IsFullwindow = false;
	inline static ImGuiID _FullwindowLastValidDockId;  // store to settings.json, presistent recover to last dock. 

	inline static VkImageView WorldImageView = nullptr;

	inline static std::vector<Imgui::DrawFuncPtr> GameDrawFuncs;

	inline static bool IsGameWindowHoveredOrFocused = false;

	inline static ImGuizmo::OPERATION	GuizmoOperation = ImGuizmo::TRANSLATE;
	inline static ImGuizmo::MODE		GuizmoMode		= ImGuizmo::WORLD;

	struct Gizmos
	{
		inline static int
			WorldGrids = 10;

		inline static bool
			ViewGizmo = true,
			ViewBasis = false,
			WorldBasis = false,
			TextInfo = false;

		inline static bool
			ChunkLoadRangeAABB = false,
			ChunksLoadingAABB = false,
			ChunksMeshingAABB = false,
			ChunksLoadedAABB = false;

	};

	static void ShowGame(bool* _open);


	static void ShowWorldSettings(bool* _open);

	static void ShowWorldNew(bool* _open);

	static void ShowWorldList(bool* _open);

	static void ShowTitleScreen(bool* _open);

};