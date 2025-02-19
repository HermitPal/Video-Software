#pragma once
#include <raylib.h>
#include <lua.hpp>

/**
 * @brief Lua binding for DrawCircle function
 * @param L Lua state pointer
 * @return Number of return values pushed onto Lua stack
 */
int l_DrawCircle(lua_State* L);

/**
 * @brief Lua binding for DrawRectangle function
 * @param L Lua state pointer
 * @return Number of return values pushed onto Lua stack
 */
int l_DrawRectangle(lua_State* L);

/**
 * @brief Lua binding for DrawText function
 * @param L Lua state pointer
 * @return Number of return values pushed onto Lua stack
 */
int l_DrawText(lua_State* L);

/**
 * @brief Lua binding for DrawLine function
 * @param L Lua state pointer
 * @return Number of return values pushed onto Lua stack
 */
int l_DrawLine(lua_State* L);

/**
 * @brief Registers all Raylib bindings with Lua
 * @param L Lua state pointer
 */
void registerRaylibBindings(lua_State* L); 