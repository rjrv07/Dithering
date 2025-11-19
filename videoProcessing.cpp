#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "Ditherer.h"

using namespace cv;

constexpr std::string inputVideoPrefix = "../input_videos/";
constexpr std::string outputVideoPrefix = "../output_videos/";
constexpr std::string palettePrefix = "../palettes/";
constexpr int numThreads = 8;
constexpr int maxQueueSize = 30; // Buffer size to prevent memory overflow

struct FrameData {
    Mat frame;
    int frameNumber;
    bool isEnd = false;
};

void processFrame(Mat &frame) {
    frame = Ditherer::gammaCorrect(frame);
    frame = Ditherer::halftone(frame, 2);
}

class FrameQueue {
    std::queue<FrameData> queue;
    std::mutex mtx;
    std::condition_variable cv;
    size_t maxSize;

public:
    explicit FrameQueue(size_t max) : maxSize(max) {}

    void push(FrameData data) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return queue.size() < maxSize; });
        queue.push(std::move(data));
        cv.notify_all();
    }

    FrameData pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !queue.empty(); });
        FrameData data = std::move(queue.front());
        queue.pop();
        cv.notify_all();
        return data;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }
};

int main() {
    std::string videoName;
    std::cout << "Enter video name: ";
    std::cin >> videoName;

    VideoCapture cap(std::string(inputVideoPrefix) + videoName + ".mp4");

    if (!cap.isOpened()) {
        std::cout << "Error: Could not open video file." << std::endl;
        return -1;
    }

    int frame_width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(CAP_PROP_FPS);
    int total_frames = static_cast<int>(cap.get(CAP_PROP_FRAME_COUNT));

    VideoWriter out(std::string(outputVideoPrefix) + videoName + " processed.mp4",
        VideoWriter::fourcc('m', 'p', '4', 'v'),
        fps,
        Size(frame_width, frame_height)
    );

    FrameQueue inputQueue(maxQueueSize);
    FrameQueue outputQueue(maxQueueSize);

    // Producer thread: reads frames from video
    std::thread producer([&]() {
        Mat frame;
        int frameNumber = 0;
        while (cap.read(frame)) {
            FrameData data;
            data.frame = frame.clone();
            data.frameNumber = frameNumber++;
            inputQueue.push(std::move(data));
        }
        // Send end signals to all worker threads
        for (int i = 0; i < numThreads; i++) {
            FrameData endData;
            endData.isEnd = true;
            inputQueue.push(std::move(endData));
        }
        std::cout << "Producer finished reading " << frameNumber << " frames." << std::endl;
    });

    // Worker threads: process frames
    std::vector<std::thread> workers;
    for (int i = 0; i < numThreads; i++) {
        workers.emplace_back([&]() {
            while (true) {
                FrameData data = inputQueue.pop();
                if (data.isEnd) break;

                processFrame(data.frame);
                outputQueue.push(std::move(data));
            }
        });
    }

    // Consumer thread: writes frames to output video in order
    std::thread consumer([&]() {
        int nextFrameNumber = 0;
        int processedCount = 0;
        std::map<int, Mat> frameBuffer; // Buffer for out-of-order frames

        while (processedCount < total_frames) {
            FrameData data = outputQueue.pop();
            if (data.isEnd) break;

            frameBuffer[data.frameNumber] = std::move(data.frame);

            // Write all consecutive frames starting from nextFrameNumber
            while (frameBuffer.count(nextFrameNumber)) {
                out.write(frameBuffer[nextFrameNumber]);
                frameBuffer.erase(nextFrameNumber);
                nextFrameNumber++;
                processedCount++;

                if (processedCount % 30 == 0) {
                    std::cout << "Processed " << processedCount << "/" << total_frames << " frames." << std::endl;
                }
            }
        }
        std::cout << "Consumer finished writing " << processedCount << " frames." << std::endl;
    });

    // Wait for all threads to complete
    producer.join();
    for (auto &worker : workers) {
        worker.join();
    }

    // Send end signal to consumer
    FrameData endData;
    endData.isEnd = true;
    outputQueue.push(std::move(endData));

    consumer.join();

    cap.release();
    out.release();
    destroyAllWindows();

    std::cout << "Video processing complete!" << std::endl;
    return 0;
}