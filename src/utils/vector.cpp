#include <cmath>
#include <algorithm>
#include "utils/vector.hpp"
#include "nlohmann/json.hpp"

void from_json(const nlohmann::json& j, Vector& vector) {
    if (!j.is_array() || j.size() != 3) {
        throw nlohmann::json::type_error::create(
            302,
            "Los vectores son arreglos de tres elementos",
            &j
        );
    }

    vector.x = j.at(0).get<float>();
    vector.y = j.at(1).get<float>();
    vector.z = j.at(2).get<float>();
}
