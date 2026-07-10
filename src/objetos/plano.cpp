#include <objetos/plano.hpp>

Plano::Plano(const nlohmann::json& j)
	: Objeto(j),
	  punto_plano(j.at("punto").get<Vector>()),
	  normal_plano(j.at("normal").get<Vector>().normal()) {}

Interseccion Plano::interseccion_mas_cercana(const Vector &p, const Vector &v) const {
	float denominador = v * normal_plano;

	// Si el denominador es casi cero, el rayo es paralelo al plano
	if (std::abs(denominador) < 1e-6f)
		return {this, std::numeric_limits<float>::infinity(), normal_plano};

	// fórmula: t = ((q - p) . n) / (v . n)
	float t = ((punto_plano - p) * normal_plano) / denominador;

	// Si t es positivo, la intersección ocurrió adelante de la cámara
	if (t > 1e-6f)
		return {this, t, normal_plano};

	return {this, std::numeric_limits<float>::infinity(), normal_plano};
}

