#pragma once

#include <utils/color.hpp>
#include <nlohmann/json.hpp>

struct Material {
	float transparencia, refractividad, reflectividad;
	Color ambiente, difusa, especular;
};

void from_json(const nlohmann::json& j, Material& m);
