
#pragma once

#include <glm/glm.hpp>


class ImwGameplay
{
public:

	// xywz = xywh.
	inline static glm::vec4 Viewport;


	static void ShowGame(bool* _open);

};