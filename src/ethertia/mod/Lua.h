
#pragma once

#include <sol/sol.hpp>

#define LUA_CTX sol::state& lua = Lua::Get();

class Lua
{
public:

	static void Init();

	static sol::state& Get();

};