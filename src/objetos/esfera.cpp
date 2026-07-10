#include <objetos/esfera.hpp>

Esfera::Esfera(const nlohmann::json& j)
	: Objeto(j),
	  centro(j.at("posicion").get<Vector>()),
	  radio(j.at("radio").get<float>()) {}

// Devuelve el menor t > 0 tal que p + tv está en el objeto.
Interseccion Esfera::interseccion_mas_cercana(const Vector &p, const Vector &v) const {
	Interseccion res = {this, std::numeric_limits<float>::infinity(), Vector()};

	// La ecuación cuadrática a resolver se obtiene desarrollando:
	//     (p + tv - centro) * (p + tv - centro) = radio ^ 2
	float c = p.get_norma_2() + centro.get_norma_2() - 2 * p.producto_interno(centro) - radio * radio,
		  b = 2 * p.producto_interno(v) - 2 * centro.producto_interno(v),
		  a = v.get_norma_2();

	float det = b * b - 4.0f * a * c;
	if (det < 0)
		return res; 

	det = sqrt(det);

	float t1 = (-b - det) / (2 * a);

	if (t1 > 1e-6) {
		res.d = t1;
		res.normal = (p + t1 * v - centro) / radio;
	} else {
		float t2 = (-b + det) / (2 * a);

		if (t2 > 1e-6) {
			res.d = t2;
			res.normal = (p + t2 * v - centro) / radio;
		}
	}

	return res;
}

