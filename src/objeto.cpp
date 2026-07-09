#include "objeto.hpp"

Objeto::Objeto(const nlohmann::json& j)
	: material(j.get<Material>()) {}

const Material &Objeto::get_material() const {
	return material;
}
