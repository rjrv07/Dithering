//
// Created by Roelie Rossouw on 2025/11/14.
//

#include "Ditherer.h"

Mat Ditherer::floydSteinberg(Palette* palette) const {
    Mat newImg = image->clone();
    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            auto oldCol = newImg.at<Vec3b>(i, j);
            newImg.at<Vec3b>(i, j) = palette->getNearestColor(newImg.at<Vec3b>(i, j));

            Vec3f err(
                (float) oldCol[0] - (float) newImg.at<Vec3b>(i, j)[0],
                (float) oldCol[1] - (float) newImg.at<Vec3b>(i, j)[1],
                (float) oldCol[2] - (float) newImg.at<Vec3b>(i, j)[2]
            );

            auto addError = [&](const int row, const int col, const float factor) {
                for (int c = 0; c < 3; c++) {
                    int val = newImg.at<Vec3b>(row, col)[c] + (int) (err[c] * factor);
                    newImg.at<Vec3b>(row, col)[c] = saturate_cast<uchar>(val);
                }
            };

            if (j + 1 < image->cols) {
                addError(i, j + 1, 7.f / 16.f);
            }
            if (i + 1 < image->rows) {
                if (j > 0) {
                    addError(i + 1, j - 1, 3.f / 16.f);
                }
                addError(i + 1, j, 5.f / 16.f);
                if (j + 1 < image->cols) {
                    addError(i + 1, j + 1, 1.f / 16.f);
                }
            }
        }
    }
    return newImg;
}

Mat Ditherer::beyer(Palette* palette, const int detail) const {
    std::vector<std::vector<int> > threshold;
    switch (detail) {
        case 1:
            threshold = {
                {0, 2},
                {3, 1}
            };
            for (auto &i: threshold) {
                for (int &j: i) {
                    j *= 64; // M[i][j] * 256 / 4
                    j -= 96; // max(M) = 3, 3 * 64 / 2
                }
            }
            break;
        case 2:
            threshold = {
                {0, 8, 2, 10},
                {12, 4, 14, 6},
                {3, 11, 1, 9},
                {15, 7, 13, 5}
            };
            for (auto &i: threshold) {
                for (int &j: i) {
                    j *= 16; // M[i][j] * 256 / 16
                    j -= 120; // max(M) = 15, 15 * 16 / 2
                }
            }
            break;
    }

    Mat newImg = image->clone();
    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            for (int c = 0; c < 3; c++) {
                int val = (int) newImg.at<Vec3b>(i, j)[c] + threshold[i % (1 << detail)][j % (1 << detail)];
                newImg.at<Vec3b>(i, j)[c] = saturate_cast<uchar>(val);
            }

            newImg.at<Vec3b>(i, j) = palette->getNearestColor(newImg.at<Vec3b>(i, j));
        }
    }
    return newImg;
}

Mat Ditherer::noDither(Palette* palette) const {
    Mat newImg = image->clone();
    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            newImg.at<Vec3b>(i, j) = palette->getNearestColor(newImg.at<Vec3b>(i, j));
        }
    }
    return newImg;
}

Mat Ditherer::halftone(const int detail) const {
    std::vector<std::vector<float>> threshold;
    std::vector<int> rowOffsets, colOffsets;
    switch (detail) {
        case 1:
            threshold = {
                {12, 4, 11, 15},
                {5, 0, 3, 10},
                {6, 1, 2, 9},
                {13, 7, 8, 14},
            };
            rowOffsets = {0, 1, 2, 0};
            colOffsets = {2, 1, 0, 0};
            break;
        case 2:
            threshold = {
                {60, 52, 51, 50, 49, 48, 59, 63},
                {53, 32, 16, 31, 30, 29, 47, 58},
                {33, 17, 12, 4, 11, 15, 28, 46},
                {34, 18, 5, 0, 3, 10, 27, 45},
                {35, 19, 6, 1, 2, 9, 26, 44},
                {36, 20, 13, 7, 8, 14, 25, 43},
                {54, 37, 21, 22, 23, 24, 42, 57},
                {61, 55, 38, 39, 40, 41, 56, 62}
            };
            rowOffsets = {0, 2, 4, 0};
            colOffsets = {4, 2, 0, 0};
            break;
    }
    for (auto &i: threshold) {
        for (float &j: i) {
            j /= threshold.size() * threshold[0].size();
        }
    }

    Mat newImg = image->clone();
    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            Vec4f color = Palette::BGRtoCMYK(newImg.at<Vec3b>(i, j));
            for (int c = 0; c < 4; c++) {
                if (color[c] > threshold[(i + rowOffsets[c]) % threshold.size()][(j + colOffsets[c]) % threshold.size()])
                    color[c] = 1;
                else color[c] = 0;
            }
            newImg.at<Vec3b>(i, j) = Palette::CMYKtoBGR(color);
        }
    }
    return newImg;
}

Mat Ditherer::gammaCorrect() const {
    Mat newImg = image->clone();
    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            for (int c = 0; c < 3; c++) {
                float val = newImg.at<Vec3b>(i, j)[c];
                val /= 255;
                val *= val;
                val *= 255;
                newImg.at<Vec3b>(i, j)[c] = saturate_cast<uchar>(val);
            }
        }
    }
    return newImg;
}
