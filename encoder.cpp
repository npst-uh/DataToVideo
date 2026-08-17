#include <iostream>
#include <vector>
#include <fstream>
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

        Encoder(std::string filename_, int width_ = 1920, int height_ = 1080, int density_ = 1, int fps_ = 60){
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

        cv::Mat createFrame(const std::vector<bool>& input){
            size_t max_bits = (static_cast<size_t>(rows) * (cols));
            if (input.size() > max_bits) {
                throw std::invalid_argument("Array bigger than frame capacity!");
            }

            cv::Mat frame = cv::Mat::zeros(height, width, CV_8UC1);
            for(int i = 0; i < input.size(); i++){
                if (input[i]) {
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
        
        void createVideo(const std::vector<bool>& inputData){
            if (!writer.isOpened()) {
                std::cerr << "Error while trying to open writer!" << std::endl;
                return;
            }
            
            size_t bitsPerFrame = static_cast<size_t>(cols) * rows;
            size_t totalBits = inputData.size();

            for (size_t i = 0; i < totalBits; i += bitsPerFrame) {
                auto begin = inputData.begin() + i;
                auto end = inputData.begin() + std::min(i + bitsPerFrame, totalBits);

                std::vector<bool> chunk(begin, end);
                cv::Mat frame = createFrame(chunk);
                writer.write(frame);
            }

            writer.release();
            std::cout << "Video successfully written to: " << filename << std::endl;
        
        }

};