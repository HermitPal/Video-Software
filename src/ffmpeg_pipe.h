#pragma once

#include <string>
#include <vector>
#include <cstdio>
#include <stdexcept>
#include <format>

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

/**
 * @class FFmpegPipe
 * @brief Handles video encoding through FFmpeg pipe interface
 */
class FFmpegPipe {
public:
    /**
     * @brief Constructs FFmpeg pipe for video encoding
     * @param width Video width in pixels
     * @param height Video height in pixels
     * @param fps Frames per second
     */
    FFmpegPipe(int width, int height, int fps);
    
    /**
     * @brief Destructor that closes the FFmpeg pipe
     */
    ~FFmpegPipe();
    
    /**
     * @brief Writes a frame to the FFmpeg pipe
     * @param buffer Vector containing RGB frame data
     */
    void writeFrame(const std::vector<unsigned char>& buffer);

private:
    FILE* pipe_ = nullptr;
}; 