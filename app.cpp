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

cv::Mat createFrame(std::vector<bool>& input, int width=1920, int height=1080, int density = 1){
    size_t max_bits = (static_cast<size_t>(width) * height)/density;
    if (input.size() > max_bits) {
        throw std::invalid_argument("Array bigger than frame capacity!");
    }

    cv::Mat frame = cv::Mat::zeros(height, width, CV_8UC3);
    for(int i = 0; i < input.size(); i++){
        int x = i % (width/density);
        int y = i / (width/density);
        if(input[i] == true){
            for(int i = 0; i < density; i++){
                for(int k = 0; k < density; k++){
                    frame.at<cv::Vec3b>((y * density + i), (x * density + k)) = cv::Vec3b(255, 255, 255);
                }
            }
        }
    }
    return frame;
}

void createVideoFromFrames(const std::vector<bool>& input, const std::string& filename, int width = 1920, int height = 1080, int density = 1, double fps = 30.0) {
    int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v'); //mp4
    
    cv::VideoWriter writer(filename, fourcc, fps, cv::Size(width, height));

    if (!writer.isOpened()) {
        std::cerr << "Error while trying to open writer!" << std::endl;
        return;
    }

    std::vector<std::vector<bool>> chunks = splitVector(input, ((width/density)*(height/density)));
    std::vector<cv::Mat> frames;
    for(int i = 0; i < chunks.size(); i++){
        cv::Mat frame = createFrame(chunks[i], width, height, density);
        writer.write(frame);
    }

    writer.release();
    std::cout << "Video erfolgreich gespeichert unter: " << filename << std::endl;
}

int main(int argc, char* argv[]){
    int width = 1920;
    int height = 1080;
    int density = 16;

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

    createVideoFromFrames(data, "output.mp4", width, height, density, 60);
}