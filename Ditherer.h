//
// Created by Roelie Rossouw on 2025/11/14.
//

#ifndef DITHERER_H
#define DITHERER_H
#include <opencv2/core/mat.hpp>

#include "Palette.h"

using namespace cv;

class Ditherer {
    static Mat floydSteinberg(Mat& image, Palette& palette);
    static Mat beyer(Mat& image, Palette& palette, int detail);
    static Mat noDither(Mat& image, Palette& palette);
    static Mat halftone(Mat& image, int detail);
    static Mat gammaCorrect(Mat& image);
};



#endif //DITHERER_H
