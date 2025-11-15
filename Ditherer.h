//
// Created by Roelie Rossouw on 2025/11/14.
//

#ifndef DITHERER_H
#define DITHERER_H
#include <opencv2/core/mat.hpp>

#include "Palette.h"

using namespace cv;

class Ditherer {
    Mat* image;

    public:
    Ditherer(Mat *image) {
        this->image = image;
    }

    [[nodiscard]] Mat floydSteinberg(Palette* palette) const;
    [[nodiscard]] Mat beyer(Palette* palette, int detail) const;
    [[nodiscard]] Mat noDither(Palette* palette) const;
    [[nodiscard]] Mat halftone(int detail) const;
    [[nodiscard]] Mat gammaCorrect() const;
};



#endif //DITHERER_H
