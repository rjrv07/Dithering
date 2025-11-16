#include <fstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

#include "Ditherer.h"
#include "Palette.h"

using namespace cv;

constexpr std::string inputImagePrefix = "../input_images/";
constexpr std::string outputImagePrefix = "../output_images/";
constexpr std::string palettePrefix = "../palettes/";

void process(Palette* palette, Ditherer* dither, bool FS, bool B1, bool B2, bool ND, bool SHOW, const std::string& imageName) {
    if (FS) {
        Mat fsDithered = dither->floydSteinberg(palette);
        if (SHOW) imshow("Floyd Steinberg: " + palette->getName(), fsDithered);
        imwrite(outputImagePrefix + imageName + " FS " + palette->getName() + ".png", fsDithered);
    }

    if (B1) {
        Mat b1Dithered = dither->beyer(palette, 1);
        if (SHOW) imshow("Beyer 1: " + palette->getName(), b1Dithered);
        imwrite(outputImagePrefix + imageName + " B1 " + palette->getName() + ".png", b1Dithered);
    }

    if (B2) {
        Mat b2Dithered = dither->beyer(palette, 2);
        if (SHOW) imshow("Beyer 2: " + palette->getName(), b2Dithered);
        imwrite(outputImagePrefix + imageName + " B2 " + palette->getName() + ".png", b2Dithered);
    }

    if (ND) {
        Mat noDithered = dither->noDither(palette);
        if (SHOW) imshow("No Dither: " + palette->getName(), noDithered);
        imwrite(outputImagePrefix + imageName + " ND " + palette->getName() + ".png", noDithered);
    }
}

int main() {
    std::cout << "Image name: ";
    std::string imageName;
    std::cin >> imageName;
    std::string imagePath = inputImagePrefix + imageName + ".png";

    Mat image = imread(imagePath, IMREAD_COLOR);
    if (image.empty()) {
        std::cout << "Could not read the image: " << imagePath << std::endl;
        return 1;
    }
    Ditherer dither(&image);

    std::cout << "Palette collection: ";
    std::string paletteName;
    std::cin >> paletteName;
    std::string palettePath = palettePrefix + paletteName + ".txt";

    char choice;

    std::cout << "Limited Depth? (y/n): ";
    std::cin >> choice;
    bool LD = choice == 'y';
    int ldDepth;
    if (LD) {
        std::cout << "Depth: ";
        std::cin >> ldDepth;
    }

    std::cout << "Halftone? (y/n): ";
    std::cin >> choice;
    bool HT = choice == 'y';
    int htDepth;
    if (HT) {
        std::cout << "Depth: ";
        std::cin >> htDepth;
    }

    std::cout << "Floyd Steinberg? (y/n): ";
    std::cin >> choice;
    bool FS = choice == 'y';

    std::cout << "Beyer 1? (y/n): ";
    std::cin >> choice;
    bool B1 = choice == 'y';

    std::cout << "Beyer 2? (y/n): ";
    std::cin >> choice;
    bool B2 = choice == 'y';

    std::cout << "No Dithering? (y/n): ";
    std::cin >> choice;
    bool ND = choice == 'y';

    std::cout << "Display output? (y/n): ";
    std::cin >> choice;
    bool SHOW = choice == 'y';

    std::ifstream fin(palettePath);
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        std::string name = line;
        getline(fin, line);

        Palette palette(name, line);

        process(&palette, &dither, FS, B1, B2, ND, SHOW, imageName);
    }

    if (LD) {
        std::vector<Vec3b> colors;
        for (float b = 0; b < 256; b += 255.f / (ldDepth - 1)) {
            for (float g = 0; g < 256; g += 255.f / (ldDepth - 1)) {
                for (float r = 0; r < 256; r += 255.f / (ldDepth - 1)) {
                    colors.emplace_back(b, g, r);
                }
            }
        }
        Palette palette("Limited Depth " + std::to_string(ldDepth) + " ", colors);

        process(&palette, &dither, FS, B1, B2, ND, SHOW, imageName);
    }

    if (HT) {
        Mat corrected = dither.gammaCorrect();
        Ditherer cDither(&corrected);
        Mat halftone = cDither.halftone(htDepth);
        if (SHOW) imshow("Halftone Image", halftone);
        imwrite(outputImagePrefix + imageName + " Halftone " + std::to_string(htDepth) + ".png", halftone);
    }

    if (SHOW) imshow("Original Image", image);

    Mat grayscaleImage = imread(imagePath, IMREAD_GRAYSCALE);
    if (SHOW) imshow("Grayscale Image", grayscaleImage);

    if (SHOW) waitKey(0);

    return 0;
}
