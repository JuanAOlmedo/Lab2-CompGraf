#include "luz.hpp"

Luz::Luz(Vector posicion, Color difusa, Color especular)
	: posicion(posicion), difusa(difusa), especular(especular) {}

Luz::Luz(const nlohmann::json& j)
	: posicion(j.at("posicion").get<Vector>()),
	  difusa(j.at("difusa").get<Color>()),
	  especular(j.at("especular").get<Color>()) {}

Vector Luz::get_posicion() const {
	return posicion;
}

Color Luz::luz_difusa() const {
	return difusa;
}

Color Luz::luz_especular() const {
	return especular;
}
