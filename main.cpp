#include <raylib.h>
#include <string>
#include <vector>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <format>
#include <lua.hpp>  // Include LuaJIT header

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

// Function to load configuration from Lua
void loadConfigFromLua(lua_State* L, int& width, int& height, int& fps, int& totalFrames, 
                      float& radius, Color& circleColor, Color& backgroundColor) {
    if (luaL_dofile(L, "main.lua") != LUA_OK) {
        throw std::runtime_error(lua_tostring(L, -1));
    }

    lua_getglobal(L, "screenWidth");
    width = luaL_checkinteger(L, -1);

    lua_getglobal(L, "screenHeight");
    height = luaL_checkinteger(L, -1);

    lua_getglobal(L, "fps");
    fps = luaL_checkinteger(L, -1);

    lua_getglobal(L, "totalFrames");
    totalFrames = luaL_checkinteger(L, -1);

    lua_getglobal(L, "circleRadius");
    radius = luaL_checknumber(L, -1);

    // Load circle color
    lua_getglobal(L, "circleColor");
    lua_getfield(L, -1, "r");
    circleColor.r = luaL_checkinteger(L, -1);
    lua_getfield(L, -2, "g");
    circleColor.g = luaL_checkinteger(L, -1);
    lua_getfield(L, -3, "b");
    circleColor.b = luaL_checkinteger(L, -1);
    circleColor.a = 255;  // Set alpha to fully opaque

    // Load background color
    lua_getglobal(L, "backgroundColor");
    lua_getfield(L, -1, "r");
    backgroundColor.r = luaL_checkinteger(L, -1);
    lua_getfield(L, -2, "g");
    backgroundColor.g = luaL_checkinteger(L, -1);
    lua_getfield(L, -3, "b");
    backgroundColor.b = luaL_checkinteger(L, -1);
    backgroundColor.a = 255;  // Set alpha to fully opaque
}

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
    Animation(int width, int height, int fps, int totalFrames, 
             float radius, Color circleColor, Color backgroundColor)
        : screenWidth_(width), screenHeight_(height), fps_(fps), 
          totalFrames_(totalFrames), circleRadius_(radius),
          circleColor_(circleColor), backgroundColor_(backgroundColor) {
        initializeRaylib();
        target_ = LoadRenderTexture(screenWidth_, screenHeight_);
        rgbBuffer_.resize(screenWidth_ * screenHeight_ * 3);
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

        for (int frame = 0; frame < totalFrames_; frame++) {
            renderFrame(circleX, circleY);
            processFrame();
            ffmpeg.writeFrame(rgbBuffer_);
            circleX += moveStep;
        }
    }

private:
    void initializeRaylib() {
        SetTraceLogLevel(LOG_NONE);
        SetConfigFlags(FLAG_WINDOW_HIDDEN);
        InitWindow(1, 1, "");
    }

    void renderFrame(float x, float y) {
        BeginTextureMode(target_);
            ClearBackground(backgroundColor_);  // Use custom background color
            DrawCircle(static_cast<int>(x), static_cast<int>(y), 
                      circleRadius_, circleColor_);  // Use custom circle color
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

    int screenWidth_;
    int screenHeight_;
    int fps_;
    int totalFrames_;
    RenderTexture2D target_;
    std::vector<unsigned char> rgbBuffer_;
    float circleRadius_;
    Color circleColor_;
    Color backgroundColor_;
};

extern "C" {
    EXPORT void runAnimation() {
        try {
            lua_State* L = luaL_newstate();
            luaL_openlibs(L);

            int width, height, fps, totalFrames;
            float radius;
            Color circleColor, backgroundColor;
            loadConfigFromLua(L, width, height, fps, totalFrames, radius, circleColor, backgroundColor);
            Animation animation(width, height, fps, totalFrames, radius, circleColor, backgroundColor);
            animation.run();

            lua_close(L);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

int main() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    int width, height, fps, totalFrames;
    float radius;
    Color circleColor, backgroundColor;
    
    try {
        loadConfigFromLua(L, width, height, fps, totalFrames, radius, circleColor, backgroundColor);
        Animation animation(width, height, fps, totalFrames, radius, circleColor, backgroundColor);
        animation.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        lua_close(L);
        return 1;
    }

    lua_close(L);
    return 0;
}