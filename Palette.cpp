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