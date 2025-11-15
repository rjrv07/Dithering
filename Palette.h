//
// Created by Roelie Rossouw on 2025/11/14.
//

#ifndef PALETTE_H
#define PALETTE_H
#include <string>
#include <utility>
#include <vector>
#include <opencv2/core/matx.hpp>

using namespace cv;

class Palette {
    std::string name;
    std::vector<Vec3b> colors;

    public:
    explicit Palette(std::string name) {
        this->name = std::move(name);
    }

    Palette(std::string name, std::vector<Vec3b> colors) {
        this->name = std::move(name);
        this->colors = std::move(colors);
    }

    Palette(std::string name, const std::string& hexCodes);
    [[nodiscard]] Vec3b getNearestColor(const Vec3b& color) const;

    [[nodiscard]] std::string getName() const {
        return name;
    }

    static Vec3b hexToVec3b(const std::string &hex);
    static double colorDistance(const Vec3b &vec, const Vec3b &ret);
};



#endif //PALETTE_H
