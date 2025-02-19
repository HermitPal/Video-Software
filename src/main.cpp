#include <iostream>
#include <lua.hpp>
#include "animation.h"
#include "raylib_bindings.h"

int main() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    registerRaylibBindings(L);
    
    try {
        if (luaL_dofile(L, "main.lua") != LUA_OK) {
            throw std::runtime_error(lua_tostring(L, -1));
        }
        
        Animation animation(L);
        animation.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        lua_close(L);
        return 1;
    }

    lua_close(L);
    return 0;
}