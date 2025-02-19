#include "raylib_bindings.h"

// Wrapper functions for Raylib
static int l_DrawCircle(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    float radius = luaL_checknumber(L, 3);
    
    luaL_checktype(L, 4, LUA_TTABLE);
    lua_getfield(L, 4, "r");
    lua_getfield(L, 4, "g");
    lua_getfield(L, 4, "b");
    lua_getfield(L, 4, "a");
    
    Color color = {
        (unsigned char)luaL_checkinteger(L, -4),
        (unsigned char)luaL_checkinteger(L, -3),
        (unsigned char)luaL_checkinteger(L, -2),
        (unsigned char)luaL_optinteger(L, -1, 255)
    };
    
    DrawCircle(x, y, radius, color);
    return 0;
}

static int l_DrawRectangle(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int width = luaL_checkinteger(L, 3);
    int height = luaL_checkinteger(L, 4);
    
    luaL_checktype(L, 5, LUA_TTABLE);
    lua_getfield(L, 5, "r");
    lua_getfield(L, 5, "g");
    lua_getfield(L, 5, "b");
    lua_getfield(L, 5, "a");
    
    Color color = {
        (unsigned char)luaL_checkinteger(L, -4),
        (unsigned char)luaL_checkinteger(L, -3),
        (unsigned char)luaL_checkinteger(L, -2),
        (unsigned char)luaL_optinteger(L, -1, 255)
    };
    
    DrawRectangle(x, y, width, height, color);
    return 0;
}

static int l_DrawText(lua_State* L) {
    const char* text = luaL_checkstring(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int fontSize = luaL_checkinteger(L, 4);
    
    luaL_checktype(L, 5, LUA_TTABLE);
    lua_getfield(L, 5, "r");
    lua_getfield(L, 5, "g");
    lua_getfield(L, 5, "b");
    lua_getfield(L, 5, "a");
    
    Color color = {
        (unsigned char)luaL_checkinteger(L, -4),
        (unsigned char)luaL_checkinteger(L, -3),
        (unsigned char)luaL_checkinteger(L, -2),
        (unsigned char)luaL_optinteger(L, -1, 255)
    };
    
    DrawText(text, x, y, fontSize, color);
    return 0;
}

static int l_DrawLine(lua_State* L) {
    int startX = luaL_checkinteger(L, 1);
    int startY = luaL_checkinteger(L, 2);
    int endX = luaL_checkinteger(L, 3);
    int endY = luaL_checkinteger(L, 4);
    
    luaL_checktype(L, 5, LUA_TTABLE);
    lua_getfield(L, 5, "r");
    lua_getfield(L, 5, "g");
    lua_getfield(L, 5, "b");
    lua_getfield(L, 5, "a");
    
    Color color = {
        (unsigned char)luaL_checkinteger(L, -4),
        (unsigned char)luaL_checkinteger(L, -3),
        (unsigned char)luaL_checkinteger(L, -2),
        (unsigned char)luaL_optinteger(L, -1, 255)
    };
    
    DrawLine(startX, startY, endX, endY, color);
    return 0;
}

void registerRaylibBindings(lua_State* L) {
    lua_newtable(L);
    
    lua_pushcfunction(L, l_DrawCircle);
    lua_setfield(L, -2, "DrawCircle");
    
    lua_pushcfunction(L, l_DrawRectangle);
    lua_setfield(L, -2, "DrawRectangle");
    
    lua_pushcfunction(L, l_DrawText);
    lua_setfield(L, -2, "DrawText");
    
    lua_pushcfunction(L, l_DrawLine);
    lua_setfield(L, -2, "DrawLine");
    
    lua_setglobal(L, "raylib");
} 