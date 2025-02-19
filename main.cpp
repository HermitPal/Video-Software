#include <raylib.h>
#include <string>
#include <vector>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <format>
#include <lua.hpp>  // Include LuaJIT header
#include "raylib_bindings.h"  // Add this include

#ifdef _WIN32
    #include <io.h>
    #include <fcntl.h>
    #define popen _popen
    #define pclose _pclose
    #define EXPORT __declspec(dllexport)
#else
    #include <unistd.h>
    #define EXPORT
#endif

class FFmpegPipe {
public:
    FFmpegPipe(int width, int height, int fps) {
        std::string ffmpegCommand = std::format(
            "ffmpeg -y -f rawvideo -pixel_format rgb24 "
            "-video_size {}x{} -framerate {} "
            "-i - -c:v libx264 -preset medium -crf 23 "
            "-pix_fmt yuv420p circle_animation.mp4",
            width, height, fps);

        #ifdef _WIN32
            _setmode(_fileno(stdin), _O_BINARY);
            _setmode(_fileno(stdout), _O_BINARY);
        #endif

        pipe_ = popen(ffmpegCommand.c_str(), "wb");
        if (!pipe_) {
            throw std::runtime_error("Failed to open FFmpeg pipe");
        }
    }

    ~FFmpegPipe() {
        if (pipe_) {
            pclose(pipe_);
        }
    }

    void writeFrame(const std::vector<unsigned char>& buffer) {
        if (fwrite(buffer.data(), buffer.size(), 1, pipe_) != 1) {
            throw std::runtime_error("Failed to write frame");
        }
    }

private:
    FILE* pipe_ = nullptr;
};

struct AnimationConfig {
    static constexpr int screenWidth = 1920;
    static constexpr int screenHeight = 1080;
    static constexpr int totalFrames = 60;
    static constexpr int fps = 30;
    static constexpr float circleRadius = 50.0f;  // Increased for higher resolution
};

class Animation {
public:
    Animation(lua_State* L)
        : L_(L),
          screenWidth_(1920),
          screenHeight_(1080), 
          fps_(30),
          totalFrames_(60),
          target_(),
          rgbBuffer_(),
          frame_(0) {
        std::cout << "Initializing Animation..." << std::endl;
        initializeRaylib();
        target_ = LoadRenderTexture(screenWidth_, screenHeight_);
        rgbBuffer_.resize(screenWidth_ * screenHeight_ * 3);
        
        // Verify Lua state
        lua_getglobal(L_, "raylib");
        if (lua_istable(L_, -1)) {
            std::cout << "Raylib table verified in Animation constructor" << std::endl;
        } else {
            std::cout << "Warning: Raylib table not found in Animation constructor" << std::endl;
        }
        lua_pop(L_, 1);
    }

    ~Animation() {
        UnloadRenderTexture(target_);
        CloseWindow();
    }

    void run() {
        FFmpegPipe ffmpeg(screenWidth_, screenHeight_, fps_);
        
        float circleX = 100.0f;  // Starting position
        const float circleY = screenHeight_ / 2.0f;
        const float moveStep = (screenWidth_ - 200.0f) / 
                             static_cast<float>(totalFrames_);

        for (frame_ = 0; frame_ < totalFrames_; frame_++) {
            renderFrame(circleX, circleY);
            processFrame();
            ffmpeg.writeFrame(rgbBuffer_);
            circleX += moveStep;
        }
    }

    void renderFrame(float x, float y) {
        BeginTextureMode(target_);
        ClearBackground(BLACK);  // Use BLACK as default background color, or any other color you prefer
        
        // Call Lua render function
        lua_getglobal(L_, "onRender");
        if (lua_isfunction(L_, -1)) {
            lua_pushnumber(L_, x);
            lua_pushnumber(L_, y);
            lua_pushnumber(L_, frame_);
            
            if (lua_pcall(L_, 3, 0, 0) != LUA_OK) {
                const char* error = lua_tostring(L_, -1);
                std::cerr << "Lua error in renderFrame: " << error << std::endl;
                
                // Debug info
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

    void processFrame() {
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

private:
    void initializeRaylib() {
        SetTraceLogLevel(LOG_NONE);
        SetConfigFlags(FLAG_WINDOW_HIDDEN);
        InitWindow(1, 1, "");
    }

    lua_State* L_;
    int screenWidth_;
    int screenHeight_;
    int fps_;
    int totalFrames_;
    RenderTexture2D target_;
    std::vector<unsigned char> rgbBuffer_;
    int frame_;
};

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