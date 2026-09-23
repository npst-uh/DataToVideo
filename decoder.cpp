#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <future>
#include <queue>
#include <cstring>
#include "decoder.hpp"

Decoder::Decoder(std::string filename_, int density){
    filename = filename_;
    this->density = density;

    //default values as fallback; are overwriten when video is read
    width = 1920;
    heigth = 1080;
    fps = 60;
    cols = width / density;
    rows = heigth / density;
    frameCount = 0;
    channels = 1;
}

Header Decoder::reconstructHeader(const cv::Mat& binaryFrame){
    std::vector<unsigned char> headerBytes(sizeof(Header), 0);
    int bitCount = std::min(
    static_cast<int>(sizeof(Header) * 8),
    cols * rows);
    
    for (int bitIndex = 0; bitIndex < bitCount; bitIndex++){
        int x = bitIndex % cols;
        int y = bitIndex / cols;

        bool bit = binaryFrame.at<unsigned char>(y * density, x * density) != 0;

        if (bit) {
            headerBytes[bitIndex / 8] |=
                static_cast<unsigned char>(1 << (7 - bitIndex % 8));
        }
    }

    Header header{};
    std::memcpy(&header, headerBytes.data(), sizeof(Header));
    return header;
}

int Decoder::reconvertFile(std::string video){
    cv::VideoCapture cap(video);
    if (!cap.isOpened()){
        std::cerr << "Video could not be opened. Check path!" << std::endl;
        return -1;
    }

    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Fehler beim Erstellen der Ausgabedatei!" << std::endl;
        return -1;
    }

    cv::Mat frame;
    frameCount = 0;

    //read out header frame first:
    cap.read(frame);
    if (frame.empty()){return -1;}
    frameCount++;
    channels = frame.channels();
    cols = frame.cols / density;
    rows = frame.rows / density;
    
    cv::Mat grayFrame;
    cv::Mat binaryFrame;
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    cv::threshold(grayFrame, binaryFrame, 128, 1, cv::THRESH_BINARY);
    Header header = reconstructHeader(binaryFrame);
    if (std::memcmp(header.magic, "D2V1", sizeof(header.magic)) != 0) {
        std::cerr << "Invalid DataToVideo header" << std::endl;
        return -1;
    }
    if (header.density != density) {
        std::cerr << "Decoder density does not match header" << std::endl;
        return -1;
    }

    std::uint64_t remaining = header.fileSize;
    while(cap.read(frame)){
        if (frame.empty()){
            break;
        }
        frameCount++;
        
        
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::threshold(grayFrame, binaryFrame, 128, 1, cv::THRESH_BINARY);
        size_t bitsPerFrame = static_cast<size_t>(cols) * rows;
        size_t bytes = bitsPerFrame / 8;
        std::vector<unsigned char> decoded(bytes, 0);
        for (size_t bitIndex = 0; bitIndex < bitsPerFrame; ++bitIndex) {
            int x = static_cast<int>(bitIndex % cols) * density;
            int y = static_cast<int>(bitIndex / cols) * density;
            if (binaryFrame.at<unsigned char>(y, x) != 0) {
                decoded[bitIndex / 8] |=
                    static_cast<unsigned char>(1 << (7 - bitIndex % 8));
            }
        }

        size_t bytesToWrite = static_cast<size_t>(
            std::min<std::uint64_t>(remaining, decoded.size()));
        outFile.write(reinterpret_cast<const char*>(decoded.data()),
                      static_cast<std::streamsize>(bytesToWrite));
        remaining -= bytesToWrite;
        if (remaining == 0) {
            break;
        }
    }
    return remaining == 0 ? 0 : -1;
}