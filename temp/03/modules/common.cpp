﻿#include "common.h"

void Common::setColor(const Cloud::Ptr &cloud, int r, int g, int b)
{
    std::uint32_t rgb = ((std::uint32_t)r<< 16 | (std::uint32_t)g << 8 | (std::uint32_t)b);
    for (auto&i:cloud->points)
        i.rgb=*reinterpret_cast<float*>(&rgb);
}

void Common::setColor(const Cloud::Ptr &cloud, const std::string &aixs)
{
    float max=-FLT_MAX;
    float min=FLT_MAX;
    float fRed = 0.f;
    float fGreen = 0.f;
    float fBlue = 0.f;
    constexpr float range = 2.f / 3.f;
    constexpr uint8_t colorMax = std::numeric_limits<uint8_t>::max();
    if (aixs=="x") {
        max = cloud->max.x;
        min = cloud->min.x;
        for (auto &i:cloud->points) {
            float hue = (max-i.x) / static_cast<float>(max - min);
            hue *= range;
            hue = range - hue;
            Tool::ConvertHSVtoRGB(hue, 1.f, 1.f, fRed, fGreen, fBlue);
            i.r = static_cast<uint8_t>(fRed * colorMax);
            i.g = static_cast<uint8_t>(fGreen * colorMax);
            i.b = static_cast<uint8_t>(fBlue * colorMax);
        }
    } else if(aixs=="y") {
        max = cloud->max.y;
        min = cloud->min.y;
        for (auto &i:cloud->points) {
            float hue = (max-i.y) / static_cast<float>(max - min);
            hue *= range;
            hue = range - hue;
            Tool::ConvertHSVtoRGB(hue, 1.f, 1.f, fRed, fGreen, fBlue);
            i.r = static_cast<uint8_t>(fRed * colorMax);
            i.g = static_cast<uint8_t>(fGreen * colorMax);
            i.b = static_cast<uint8_t>(fBlue * colorMax);
        }
    } else if(aixs=="z") {
        max = cloud->max.z;
        min = cloud->min.z;
        for (auto &i:cloud->points) {
            float hue = (max-i.z) / static_cast<float>(max - min);
            hue *= range;
            hue = range - hue;
            Tool::ConvertHSVtoRGB(hue, 1.f, 1.f, fRed, fGreen, fBlue);
            i.r = static_cast<uint8_t>(fRed * colorMax);
            i.g = static_cast<uint8_t>(fGreen * colorMax);
            i.b = static_cast<uint8_t>(fBlue * colorMax);
        }
    }
}
