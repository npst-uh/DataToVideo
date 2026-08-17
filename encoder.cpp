#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <future>
#include <queue>
#include "encoder.hpp"

Encoder::Encoder(std::string filename_, int width_, int height_, int density_, int fps_){
    filename = filename_;
    width = width_;
    height = height_;
    density = density_;
    fps = fps_;
    cols = width / density;
    rows = height / density;
    int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v'); //mp4
    writer = cv::VideoWriter(filename, fourcc, fps, cv::Size(width, height));
}

cv::Mat Encoder::createFrame(const std::vector<char>& buffer, std::streamsize bytesRead){
    size_t max_bits = (static_cast<size_t>(rows) * (cols));
    cv::Mat frame = cv::Mat::zeros(height, width, CV_8UC1);
    size_t totalBits = static_cast<size_t>(bytesRead) * 8;

    for(int i = 0; i < totalBits; ++i){
        uint8_t byte = static_cast<uint8_t>(buffer[i / 8]);
        bool bit = (byte >> (7 - (i % 8))) & 1;
        if (bit) {
            int x = (i % cols) * density;
            int y = (i / cols) * density;
            cv::Rect block(x, y, density, density);
            frame(block).setTo(255);
        }
    }
    cv::Mat colorFrame;
    cv::cvtColor(frame, colorFrame, cv::COLOR_GRAY2BGR);
    return colorFrame;
}
        
void Encoder::createVideo(std::ifstream& file){
    if (!file.is_open()) {
        std::cerr << "Not able to open file" << std::endl;
        return;
    }

    if (!writer.isOpened()) {
        std::cerr << "Error while trying to open writer!" << std::endl;
        return;
    }

    const size_t BUFFER_SIZE = std::thread::hardware_concurrency(); 
    std::queue<std::future<cv::Mat>> frameQueue;

    size_t bitsPerFrame = static_cast<size_t>(cols) * rows;
    size_t bytesPerFrame = bitsPerFrame / 8;

    while(true){
        while(frameQueue.size() < BUFFER_SIZE){
            std::vector<char> buffer(bytesPerFrame);
            if (!(file.read(buffer.data(), bytesPerFrame) || file.gcount() > 0)) {
                break; //End fo file reached
            }
            std::streamsize bytesRead = file.gcount();
            
            frameQueue.push(std::async(std::launch::async, [this, buf = std::move(buffer), bytesRead](){
                return createFrame(buf, bytesRead);
            }));
        }

        if (frameQueue.empty()) {
            break;
        }
        
        cv::Mat frame = frameQueue.front().get();
        writer.write(frame);
        frameQueue.pop();
    }

    writer.release();
    std::cout << "Video successfully written to: " << filename << std::endl;

}
