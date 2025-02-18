#include <raylib.h>
#include <string>
#include <vector>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <format>

#ifdef _WIN32
    #include <io.h>
    #include <fcntl.h>
    #define popen _popen
    #define pclose _pclose
#else
    #include <unistd.h>
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
    Animation() {
        initializeRaylib();
        target_ = LoadRenderTexture(AnimationConfig::screenWidth, AnimationConfig::screenHeight);
        rgbBuffer_.resize(AnimationConfig::screenWidth * AnimationConfig::screenHeight * 3);
    }

    ~Animation() {
        UnloadRenderTexture(target_);
        CloseWindow();
    }

    void run() {
        FFmpegPipe ffmpeg(AnimationConfig::screenWidth, AnimationConfig::screenHeight, AnimationConfig::fps);
        
        float circleX = 100.0f;  // Starting position
        const float circleY = AnimationConfig::screenHeight / 2.0f;
        const float moveStep = (AnimationConfig::screenWidth - 200.0f) / 
                             static_cast<float>(AnimationConfig::totalFrames);

        for (int frame = 0; frame < AnimationConfig::totalFrames; frame++) {
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
            ClearBackground(RAYWHITE);
            DrawCircle(static_cast<int>(x), static_cast<int>(y), 
                      AnimationConfig::circleRadius, RED);
        EndTextureMode();
    }

    void processFrame() {
        Image image = LoadImageFromTexture(target_.texture);
        ImageFlipVertical(&image);
        
        for (int y = 0; y < AnimationConfig::screenHeight; y++) {
            for (int x = 0; x < AnimationConfig::screenWidth; x++) {
                Color pixel = GetImageColor(image, x, y);
                const size_t index = (y * AnimationConfig::screenWidth + x) * 3;
                rgbBuffer_[index] = pixel.r;
                rgbBuffer_[index + 1] = pixel.g;
                rgbBuffer_[index + 2] = pixel.b;
            }
        }
        
        UnloadImage(image);
    }

    RenderTexture2D target_;
    std::vector<unsigned char> rgbBuffer_;
};

int main() {
    try {
        Animation animation;
        animation.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}