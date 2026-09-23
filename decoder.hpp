#ifndef DECODER_HPP
#define DECODER_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "format.hpp"

class Decoder {
    private:
        int width;
        int heigth;
        int density;
        int cols;
        int rows;
        int fps;
        int channels;
        std::string filename;
        int frameCount;

    public:
        Decoder(std::string filename_, int density);
        Header reconstructHeader(const cv::Mat& binaryFrame);
        int reconvertFile(std::string video);
};

#endif // DECODER_HPP