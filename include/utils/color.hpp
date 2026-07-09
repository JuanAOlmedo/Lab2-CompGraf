#pragma once

#include "nlohmann/json.hpp"

struct Color {
    float r, g, b;

    Color operator*(const Color& other) const;
    Color operator*(float x) const;
    Color operator+(const Color& other) const;

    void operator*=(float x);
    void operator*=(const Color& other);
    void operator+=(const Color& other);
    void operator-=(const Color& other);
};

void from_json(const nlohmann::json& j, Color& color);
