#include <objetos/esfera.hpp>

Esfera::Esfera(const nlohmann::json& j)
	: Objeto(j),
	  centro(j.at("posicion").get<Vector>()),
	  radio(j.at("radio").get<float>()) {}

// Devuelve el menor t > 0 tal que p + tv está en el objeto.
float Esfera::interseccion_mas_cercana(const Vector &p, const Vector &v) const {
	// La ecuación cuadrática a resolver se obtiene desarrollando:
	//     (p + tv - centro) * (p + tv - centro) = radio ^ 2
	float c = p.get_norma_2() + centro.get_norma_2() - 2 * p.producto_interno(centro) - radio * radio,
		  b = 2 * p.producto_interno(v) - 2 * centro.producto_interno(v),
		  a = v.get_norma_2();

	float det = b * b - 4.0f * a * c;
	if (det < 0)
		return -1; 

	det = sqrt(det);

	float t1 = (-b - det) / (2 * a);

	if (t1 > 1e-6)
		return t1;

	float t2 = (-b + det) / (2 * a);

	if (t2 > 1e-6)
		return t2;

	return -1;
}

Vector Esfera::normal_en_punto(const Vector &p) const {
	return (p - centro) / radio;
}
