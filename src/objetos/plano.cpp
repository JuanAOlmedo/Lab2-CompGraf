#include <objetos/plano.hpp>

Plano::Plano(Vector punto, Vector normal, bool reflejante, bool transparencia,
			 Color ambiente, Color difusa, Color especular)
	: Objeto(reflejante, transparencia, 1, ambiente, difusa, especular),
	  punto_plano(punto), normal_plano(normal.normal()) {}

Plano::Plano(const nlohmann::json& j)
	: Objeto(j),
	  punto_plano(j.at("punto").get<Vector>()),
	  normal_plano(j.at("normal").get<Vector>().normal()) {}

float Plano::interseccion_mas_cercana(const Vector &p, const Vector &v) const {
	float denominador = v * normal_plano;

	// Si el denominador es casi cero, el rayo es paralelo al plano
	if (std::abs(denominador) < 1e-6f) {
		return -1.0f;
	}

	// fórmula: t = ((q - p) . n) / (v . n)
	float t = ((punto_plano - p) * normal_plano) / denominador;

	// Si t es positivo, la intersección ocurrió adelante de la cámara
	if (t > 1e-6f)
		return t;

	return -1.0f;
}

Vector Plano::normal_en_punto(const Vector &) const {
	// La normal de un plano es constante en toda su superficie
	return normal_plano;
}
