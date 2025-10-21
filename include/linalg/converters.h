#pragma once

#include <Eigen/Core>
#include "linalg/linalg.h"

linalg::Vec3<float> EigenToLinalg(const Eigen::Vector3f &v)
{
    return linalg::Vec3<float>(v.x(), v.y(), v.z());
}

linalg::Mat3<float> EigenToLinalg(const Eigen::Matrix3f &m)
{
    linalg::Mat3<float> lm;
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            lm(r, c) = m(r, c);
    return lm;
}

linalg::Mat4<float> EigenToLinalg(const Eigen::Matrix4f &m)
{
    linalg::Mat4<float> lm;
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            lm(r, c) = m(r, c);
    return lm;
}