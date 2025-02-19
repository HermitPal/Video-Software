#include "animation.h"
#include <iostream>

Animation::Animation(lua_State* L)
    : L_(L),
      screenWidth_(1920),
      screenHeight_(1080), 
      fps_(30),
      totalFrames_(600),
      target_(),
      rgbBuffer_(),
      frame_(0) {
    std::cout << "Initializing Animation..." << std::endl;
    initializeRaylib();
    target_ = LoadRenderTexture(screenWidth_, screenHeight_);
    rgbBuffer_.resize(screenWidth_ * screenHeight_ * 3);
    
    lua_getglobal(L_, "raylib");
    if (lua_istable(L_, -1)) {
        std::cout << "Raylib table verified in Animation constructor" << std::endl;
    } else {
        std::cout << "Warning: Raylib table not found in Animation constructor" << std::endl;
    }
    lua_pop(L_, 1);
}

Animation::~Animation() {
    UnloadRenderTexture(target_);
    CloseWindow();
}

void Animation::run() {
    FFmpegPipe ffmpeg(screenWidth_, screenHeight_, fps_);
    
    float circleX = 100.0f;
    const float circleY = screenHeight_ / 2.0f;
    const float moveStep = (screenWidth_ - 200.0f) / static_cast<float>(totalFrames_);

    for (frame_ = 0; frame_ < totalFrames_; frame_++) {
        renderFrame(circleX, circleY);
        processFrame();
        ffmpeg.writeFrame(rgbBuffer_);
        circleX += moveStep;
    }
}

void Animation::renderFrame(float x, float y) {
    BeginTextureMode(target_);
    ClearBackground(BLACK);
    
    lua_getglobal(L_, "onRender");
    if (lua_isfunction(L_, -1)) {
        lua_pushnumber(L_, x);
        lua_pushnumber(L_, y);
        lua_pushnumber(L_, frame_);
        
        if (lua_pcall(L_, 3, 0, 0) != LUA_OK) {
            const char* error = lua_tostring(L_, -1);
            std::cerr << "Lua error in renderFrame: " << error << std::endl;
            
            lua_getglobal(L_, "raylib");
            if (lua_istable(L_, -1)) {
                std::cerr << "raylib table exists at error time" << std::endl;
            } else {
                std::cerr << "raylib table missing at error time" << std::endl;
            }
            lua_pop(L_, 1);
            
            throw std::runtime_error(std::string("Lua error: ") + error);
        }
    }
    
    EndTextureMode();
}

void Animation::processFrame() {
    Image image = LoadImageFromTexture(target_.texture);
    ImageFlipVertical(&image);
    
    for (int y = 0; y < screenHeight_; y++) {
        for (int x = 0; x < screenWidth_; x++) {
            Color pixel = GetImageColor(image, x, y);
            const size_t index = (y * screenWidth_ + x) * 3;
            rgbBuffer_[index] = pixel.r;
            rgbBuffer_[index + 1] = pixel.g;
            rgbBuffer_[index + 2] = pixel.b;
        }
    }
    
    UnloadImage(image);
}

void Animation::initializeRaylib() {
    SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(1, 1, "");
} 