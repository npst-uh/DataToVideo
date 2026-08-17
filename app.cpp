#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <encoder.cpp>

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

    if (!file.is_open()) {
        std::cerr << "Not able to open file" << std::endl;
        return 1;
    }
    
    std::vector<bool> data;
    char byte;
    
    while(file.get(byte)) {
        for(int i = 7; i>=0; i--){
            bool bit = (byte >> i) & 1;
            data.push_back(bit);
        }
    }

    file.close();

    encoder.createVideo(data);
}