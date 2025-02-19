#pragma once
#include <raylib.h>
#include <lua.hpp>

// Declare the Lua binding functions
int l_DrawCircle(lua_State* L);
int l_DrawRectangle(lua_State* L);
int l_DrawText(lua_State* L);
int l_DrawLine(lua_State* L);

// Function to register all Raylib bindings
void registerRaylibBindings(lua_State* L); 