#include <iostream>
#include <lua.hpp>

// Example function we'll expose to Lua
static int sayHello(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1); // Get first argument as string
    std::cout << "Hello, " << name << "!!!!" << std::endl;
    return 0; // number of return values
}

int main()
{
    // Create new Lua state
    lua_State *L = luaL_newstate();
    luaL_openlibs(L); // Load Lua libraries

    // Register our C++ function in Lua
    lua_pushcfunction(L, sayHello);
    lua_setglobal(L, "sayHello");

    // Load and run the Lua script from a file
    const char *luaFile = "script.lua";
    if (luaL_dofile(L, luaFile) != LUA_OK)
    {
        std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        lua_close(L);
        return 1;
    }

    // Call Lua function from C++
    lua_getglobal(L, "greetFromLua");
    if (lua_pcall(L, 0, 1, 0) != LUA_OK)
    {
        std::cerr << "Error calling Lua function: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
    else
    {
        int result = lua_tonumber(L, -1);
        std::cout << "Got value from Lua: " << result << std::endl;
        lua_pop(L, 1);
    }

    // Clean up
    lua_close(L);
    return 0;
}