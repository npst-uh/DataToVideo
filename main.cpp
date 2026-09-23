#include <iostream>
#include <fstream>
#include <filesystem>
#include "encoder.hpp"
#include "decoder.hpp"

int main(int argc, char* argv[]){
    const int width = 1920;
    const int height = 1080;
    const int density = 16;
    const int fps = 60;
    if (argc >= 2 && std::string(argv[1]) == "--decode") {
        if (argc < 4) {
            std::cerr << "Usage: " << argv[0]
                      << " --decode <video> <output-file>" << std::endl;
            return 1;
        }
        Decoder decoder(argv[3], density);
        return decoder.reconvertFile(argv[2]);
    }

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0]
                  << " <input-file> | --decode <video> <output-file>"
                  << std::endl;
        return 1;
    }
    Encoder encoder("output.mp4", width, height, density, fps);
    std::ifstream file(argv[1], std::ios::binary);

    std::string fileExtension = std::filesystem::path(argv[1]).extension().string();
    encoder.createVideo(file, fileExtension);

    file.close();
    return 0;
}