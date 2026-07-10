#include <objetos/malla.hpp>

struct Cara {
    int v0, v1, v2;
};

void from_json(const nlohmann::json& j, Cara& c) {
    if (!j.is_array() || j.size() != 3) {
        throw nlohmann::json::type_error::create(
            302,
            "Los cuadriláteros son arreglos de cuatro elementos",
            &j
        );
    }

    c.v0 = j.at(0).get<float>();
    c.v1 = j.at(1).get<float>();
    c.v2 = j.at(2).get<float>();
}

float Malla::interseccion_triangulo(const Vector& p, const Vector& v,
		const Vector& v0, const Vector& v1, const Vector& v2, Vector& normal_out) {
	const float EPSILON = 1e-6;
	Vector edge1 = v1 - v0;
	Vector edge2 = v2 - v0;

	Vector h = v.producto_vectorial(edge2);
	float det = edge1.producto_interno(h);

	if (det < EPSILON)
		return -1.0f;

	float inv_det = 1.0f / det;
	Vector s = p - v0;
	float u = inv_det * s.producto_interno(h);
	if (u < 0.0f - EPSILON || u > 1.0f + EPSILON)
		return -1.0f;

	Vector q = s.producto_vectorial(edge1);
	float _v = inv_det * v.producto_interno(q);
	if (_v < 0.0f - EPSILON || u + _v > 1.0f + EPSILON)
		return -1.0f;

	float t = inv_det * edge2.producto_interno(q);
	if (t > EPSILON) {
		normal_out = edge1.producto_vectorial(edge2).normal();
		return t;
	}
	
	return -1.0f;
}

Malla::Malla(const nlohmann::json& j)
	: Objeto(j),
	  vertices(j.at("vertices").get<std::vector<Vector>>()),
	  caras(j.at("caras").get<std::vector<Cara>>()) {}

Interseccion Malla::interseccion_mas_cercana(const Vector &p, const Vector &v) const {
	float t_min = std::numeric_limits<float>::infinity();
	Vector normal;

	for (const auto& cara : caras) {
		Vector p0 = vertices[cara.v0];
		Vector p1 = vertices[cara.v1];
		Vector p2 = vertices[cara.v2];

		Vector temp;
		float t = interseccion_triangulo(p, v, p0, p1, p2, temp);
		if (t > 1e-4f && t < t_min - 1e-4) {
			t_min = t;
			normal = temp;
		}
	}

	return {this, t_min, normal};
}

