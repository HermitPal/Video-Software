#pragma once

#include <raylib.h>
#include <vector>
#include <lua.hpp>
#include "ffmpeg_pipe.h"

/**
 * @class Animation
 * @brief Handles rendering and processing of animation frames using Raylib and Lua
 */
class Animation {
public:
    /**
     * @brief Constructs an Animation object
     * @param L Pointer to Lua state
     */
    Animation(lua_State* L);
    
    /**
     * @brief Destructor that cleans up Raylib resources
     */
    ~Animation();
    
    /**
     * @brief Executes the main animation loop
     */
    void run();

private:
    /**
     * @brief Renders a single frame of animation
     * @param x X-coordinate for rendering
     * @param y Y-coordinate for rendering
     */
    void renderFrame(float x, float y);
    
    /**
     * @brief Processes the rendered frame for output
     */
    void processFrame();
    
    /**
     * @brief Initializes Raylib with required settings
     */
    void initializeRaylib();

    lua_State* L_;
    int screenWidth_;
    int screenHeight_;
    int fps_;
    int totalFrames_;
    RenderTexture2D target_;
    std::vector<unsigned char> rgbBuffer_;
    int frame_;
}; 