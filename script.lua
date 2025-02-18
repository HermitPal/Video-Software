print("Lua is initialized!")

-- Call our C++ function
sayHello("from Lua")

-- Define a Lua function
function greetFromLua()
    print("Greetings from Lua!")
    return 67
end