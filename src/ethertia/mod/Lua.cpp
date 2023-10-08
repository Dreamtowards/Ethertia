

#include "Lua.h"

static sol::state g_LuaState;


void Lua::Init()
{
	LUA_CTX;

	lua.open_libraries(sol::lib::base, sol::lib::package);


}

sol::state& Lua::Get()
{
	return g_LuaState;
}