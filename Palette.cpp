//
// Created by Roelie Rossouw on 2025/11/14.
//

#include "Palette.h"
#include "Utilities.h"
typedef Utilities util;

#include <sstream>
#include <utility>

Palette::Palette(std::string name, const std::string &hexCodes) {
    this->name = std::move(name);
    std::istringstream iss(hexCodes);
    std::string hexCode;
    while (getline(iss, hexCode, ',')) {
        colors.push_back(util::hexToVec3b(hexCode));
    }
}

Vec3b Palette::getNearestColor(const Vec3b &color) const {
    Vec3b nearest = colors[0];
    double minDistance = util::dist(color, nearest);
    for (const auto &current: colors) {
        if (const double curD = util::dist(color, current); curD < minDistance) {
            nearest = current;
            minDistance = curD;
        }
    }
    return nearest;
}
