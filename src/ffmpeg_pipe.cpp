#include "ffmpeg_pipe.h"

FFmpegPipe::FFmpegPipe(int width, int height, int fps) {
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

FFmpegPipe::~FFmpegPipe() {
    if (pipe_) {
        pclose(pipe_);
    }
}

void FFmpegPipe::writeFrame(const std::vector<unsigned char>& buffer) {
    if (fwrite(buffer.data(), buffer.size(), 1, pipe_) != 1) {
        throw std::runtime_error("Failed to write frame");
    }
} 