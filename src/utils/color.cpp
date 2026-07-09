#include "utils/color.hpp"

Color Color::operator*(const Color& other) const {
	return {
		std::min(r * other.r / 255.0f, 255.0f),
		std::min(g * other.g / 255.0f, 255.0f),
		std::min(b * other.b / 255.0f, 255.0f)
	};
}

Color Color::operator*(float x) const {
	return {
		std::min(r * x, 255.0f),
		std::min(g * x, 255.0f),
		std::min(b * x, 255.0f)
	};
}

Color Color::operator+(const Color& other) const {
	return {
		std::min(r + other.r, 255.0f),
		std::min(g + other.g, 255.0f),
		std::min(b + other.b, 255.0f)
	};
}

void Color::operator*=(float x) {
	r = std::min(r * x, 255.0f);
	g = std::min(g * x, 255.0f);
	b = std::min(b * x, 255.0f);
}

void Color::operator*=(const Color& other) {
	r = std::min(r * other.r / 255.0f, 255.0f);
	g = std::min(g * other.g / 255.0f, 255.0f);
	b = std::min(b * other.b / 255.0f, 255.0f);
}

void Color::operator+=(const Color& other) {
	r = std::min(r + other.r, 255.0f);
	g = std::min(g + other.g, 255.0f);
	b = std::min(b + other.b, 255.0f);
}

void Color::operator-=(const Color& other) {
	r = std::max(r - other.r, 0.0f);
	g = std::max(g - other.g, 0.0f);
	b = std::max(b - other.b, 0.0f);
}

void from_json(const nlohmann::json& j, Color& color) {
    if (!j.is_array() || j.size() != 3) {
        throw nlohmann::json::type_error::create(
            302,
            "Los colores son arreglos de tres elementos",
            &j
        );
    }

    color.r = j.at(0).get<float>();
    color.g = j.at(1).get<float>();
    color.b = j.at(2).get<float>();

    if (color.r < 0 || 255 < color.r || color.g < 0 || 255 < color.g
    	|| color.b < 0 || 255 < color.b) {
    	throw nlohmann::json::type_error::create(
            302,
            "Los colores tienen que estar en el rango 0-255",
            &j
        );
    }
}
