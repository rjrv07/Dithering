//
// Created by Roelie Rossouw on 2025/11/14.
//

#ifndef DITHERER_H
#define DITHERER_H
#include <opencv2/core/mat.hpp>

#include "Palette.h"

using namespace cv;

class Ditherer {
    Palette* palette;
    Mat* image;

    public:
    Ditherer(Palette *palette, Mat *image) {
        this->palette = palette;
        this->image = image;
    }

    [[nodiscard]] Mat floydSteinberg() const;
    [[nodiscard]] Mat beyer(int detail) const;
    [[nodiscard]] Mat noDither() const;
};



#endif //DITHERER_H
