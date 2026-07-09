#include "utils/color.hpp"

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
