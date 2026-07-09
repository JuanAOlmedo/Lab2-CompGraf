#include "objeto.hpp"

Objeto::Objeto(bool reflejante, float transparencia, float refraccion,
	   Color ambiente, Color difusa, Color especular)
	: reflejante(reflejante), transparencia(transparencia), refraccion(refraccion),
	  ambiente(ambiente), difusa(difusa), especular(especular) {}
Objeto::Objeto(const nlohmann::json& j)
	: reflejante(j.at("espejado").get<bool>()),
	  transparencia(j.at("transparencia").get<float>()),
	  refraccion(j.at("refraccion").get<float>()),
	  ambiente(j.at("luz").at("ambiente").get<Color>()),
	  difusa(j.at("luz").at("difusa").get<Color>()),
	  especular(j.at("luz").at("especular").get<Color>()) {}

// Devuelven si el objeto es reflejante y su transparencia
bool Objeto::get_reflejante() const {
	return reflejante;
}

float Objeto::get_transparencia() const {
	return transparencia;
}

float Objeto::get_refraccion() const {
	return refraccion;
}

// Componentes de luz
Color Objeto::luz_ambiente() const {
	return ambiente;
}

Color Objeto::luz_difusa() const {
	return difusa;
}

Color Objeto::luz_especular() const {
	return especular;
}
