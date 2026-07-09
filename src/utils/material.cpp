#include <utils/material.hpp>

void from_json(const nlohmann::json& j, Material& m) {
	m.reflectividad = j.at("espejado").get<float>();
	m.transparencia = j.at("transparencia").get<float>();
	m.refractividad = j.at("refraccion").get<float>();
	m.ambiente = j.at("luz").at("ambiente").get<Color>();
	m.difusa = j.at("luz").at("difusa").get<Color>();
	m.especular = j.at("luz").at("especular").get<Color>();
}
