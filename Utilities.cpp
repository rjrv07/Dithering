//
// Created by Roelie Rossouw on 2025/11/14.
//

#include "Utilities.h"

Vec3b Utilities::hexToVec3b(const std::string &hex) {
    auto toByte = [](const std::string &s) -> uchar {
        return static_cast<uchar>(std::stoi(s, nullptr, 16));
    };

    Vec3b col;
    col[0] = toByte(hex.substr(4, 2)); // B
    col[1] = toByte(hex.substr(2, 2)); // G
    col[2] = toByte(hex.substr(0, 2)); // R
    return col;
}

double Utilities::dist(const Vec3b &vec, const Vec3b &ret) {
    double sum = 0;
    for (int i = 0; i < 3; i++) {
        int diff = (int) vec[i] - (int) ret[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}