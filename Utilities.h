//
// Created by Roelie Rossouw on 2025/11/14.
//

#ifndef UTILITIES_H
#define UTILITIES_H
#include <string>
#include <opencv2/core/matx.hpp>

using namespace cv;

class Utilities {
public:
    static Vec3b hexToVec3b(const std::string &hex);
    static double dist(const Vec3b &vec, const Vec3b &ret);
};



#endif //UTILITIES_H
