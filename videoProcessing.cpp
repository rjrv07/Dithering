#include <opencv2/opencv.hpp>
#include <iostream>

#include "Ditherer.h"

using namespace cv;

constexpr std::string inputVideoPrefix = "../input_videos/";
constexpr std::string outputVideoPrefix = "../output_videos/";
constexpr std::string palettePrefix = "../palettes/";

int main() {
    std::string videoName;
    std::cout << "Enter video name: ";
    std::cin >> videoName;
    // Open the video file
    VideoCapture cap(inputVideoPrefix + videoName + ".mp4");

    // Check if the video was opened successfully
    if (!cap.isOpened()) {
        std::cout << "Error: Could not open video file." << std::endl;
        return -1;
    }

    // Get frame width and height
    int frame_width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));

    // Define the codec and create VideoWriter object
    VideoWriter out(inputVideoPrefix + videoName + " processed.mp4",
        VideoWriter::fourcc('m', 'p', '4', 'v'),
    cap.get(CAP_PROP_FPS),
        Size(frame_width, frame_height)
    );

    Mat frame;
    while (cap.read(frame)) {
        frame = Ditherer::gammaCorrect(frame);
        frame = Ditherer::halftone(frame, 2);

        // Write the frame to the output video file
        out.write(frame);

        // Display the frame
        imshow("Frame", frame);
        if (waitKey(1) == 'q') {
            break;
        }
    }

    // Release everything
    cap.release();
    out.release();
    destroyAllWindows();
    return 0;
}
