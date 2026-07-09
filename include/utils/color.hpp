#pragma once

#include "nlohmann/json.hpp"
#include <algorithm>

struct Color {
    float r, g, b;

    Color operator*(const Color& other) const {
        return {
            std::min(r * other.r / 255.0f, 255.0f),
            std::min(g * other.g / 255.0f, 255.0f),
            std::min(b * other.b / 255.0f, 255.0f)
        };
    }

    Color operator*(float x) const {
        return {
            std::min(r * x, 255.0f),
            std::min(g * x, 255.0f),
            std::min(b * x, 255.0f)
        };
    }

    Color operator+(const Color& other) const {
        return {
            std::min(r + other.r, 255.0f),
            std::min(g + other.g, 255.0f),
            std::min(b + other.b, 255.0f)
        };
    }

    void operator*=(float x) {
        r = std::min(r * x, 255.0f);
        g = std::min(g * x, 255.0f);
        b = std::min(b * x, 255.0f);
    }

    void operator*=(const Color& other) {
        r = std::min(r * other.r / 255.0f, 255.0f);
        g = std::min(g * other.g / 255.0f, 255.0f);
        b = std::min(b * other.b / 255.0f, 255.0f);
    }

    void operator+=(const Color& other) {
        r = std::min(r + other.r, 255.0f);
        g = std::min(g + other.g, 255.0f);
        b = std::min(b + other.b, 255.0f);
    }

    void operator-=(const Color& other) {
        r = std::max(r - other.r, 0.0f);
        g = std::max(g - other.g, 0.0f);
        b = std::max(b - other.b, 0.0f);
    }
};

void from_json(const nlohmann::json& j, Color& color);
