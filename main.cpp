#include <iostream>
#include <fstream>
#include "encoder.hpp"

int main(int argc, char* argv[]){
    int width = 1920;
    int height = 1080;
    int density = 16;
    int fps = 60;
    Encoder encoder = Encoder("output.mp4", width, height, density, fps);

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input-file>" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1], std::ios::binary);

    encoder.createVideo(file);

    file.close();
    return 0;
}