#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <opencv2/opencv.hpp>

template <typename T>
std::vector<std::vector<T>> splitVector(const std::vector<T>& input, size_t chunkSize) {
    std::vector<std::vector<T>> chunks;
    if (chunkSize == 0) return chunks;

    for (size_t i = 0; i < input.size(); i += chunkSize) {
        auto begin = input.begin() + i;
        auto end = input.begin() + std::min(i + chunkSize, input.size());
        chunks.emplace_back(begin, end);
    }
    return chunks;
}

class Encoder {
private:
    int width;
    int height;
    int density;
    int cols;
    int rows;
    int fps;
    std::string filename;
    cv::VideoWriter writer;

public:
    Encoder(std::string filename_, int width_ = 1920, int height_ = 1080, int density_ = 1, int fps_ = 60);
    cv::Mat createFrame(const std::vector<char>& buffer, std::streamsize bytesRead);
    void createVideo(std::ifstream& file, const std::string& fileExtension);
};

#endif // ENCODER_HPP