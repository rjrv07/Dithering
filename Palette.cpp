//
// Created by Roelie Rossouw on 2025/11/14.
//

#include "Palette.h"

#include <sstream>
#include <utility>

Palette::Palette(std::string name, const std::string &hexCodes) {
    this->name = std::move(name);
    std::istringstream iss(hexCodes);
    std::string hexCode;
    while (getline(iss, hexCode, ',')) {
        colors.push_back(hexToVec3b(hexCode));
    }
}

Vec3b Palette::getNearestColor(const Vec3b &color) const {
    Vec3b nearest = colors[0];
    double minDistance = colorDistance(color, nearest);
    for (const auto &current: colors) {
        if (const double curD = colorDistance(color, current); curD < minDistance) {
            nearest = current;
            minDistance = curD;
        }
    }
    return nearest;
}

Vec3b Palette::hexToVec3b(const std::string &hex) {
    auto toByte = [](const std::string &s) -> uchar {
        return static_cast<uchar>(std::stoi(s, nullptr, 16));
    };

    Vec3b col;
    col[0] = toByte(hex.substr(4, 2)); // B
    col[1] = toByte(hex.substr(2, 2)); // G
    col[2] = toByte(hex.substr(0, 2)); // R
    return col;
}

double Palette::colorDistance(const Vec3b &vec, const Vec3b &ret) {
    double sum = 0;
    for (int i = 0; i < 3; i++) {
        int diff = (int) vec[i] - (int) ret[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

Vec4f Palette::BGRtoCMYK(const Vec3b &color) {
    Vec4f cmyk;
    float b = color[0] / 255.f, g = color[1] / 255.f, r = color[2] / 255.f;
    cmyk[3] = 1 - max(max(b, g), r);
    cmyk[0] = (1 - r - cmyk[3]) / (1 - cmyk[3]);
    cmyk[1] = (1 - g - cmyk[3]) / (1 - cmyk[3]);
    cmyk[2] = (1 - b - cmyk[3]) / (1 - cmyk[3]);
    return cmyk;
}

Vec3b Palette::CMYKtoBGR(const Vec4f &color) {
    Vec3b bgr;
    float c = color[0] * (1 - color[3]) + color[3];
    float m = color[1] * (1 - color[3]) + color[3];
    float y = color[2] * (1 - color[3]) + color[3];

    float r = (1 - c) * 255;
    float g = (1 - m) * 255;
    float b = (1 - y) * 255;
    bgr[0] = b; bgr[1] = g; bgr[2] = r;
    return bgr;
}
