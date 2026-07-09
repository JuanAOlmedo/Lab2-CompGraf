#include <objetos/malla.hpp>

struct Cara {
    int v0, v1, v2, v3; // Los 4 índices de los vértices en sentido horario o antihorario
};

void from_json(const nlohmann::json& j, Cara& c) {
    if (!j.is_array() || j.size() != 4) {
        throw nlohmann::json::type_error::create(
            302,
            "Los cuadriláteros son arreglos de cuatro elementos",
            &j
        );
    }

    c.v0 = j.at(0).get<float>();
    c.v1 = j.at(1).get<float>();
    c.v2 = j.at(2).get<float>();
    c.v3 = j.at(3).get<float>();
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

Malla::Malla(const std::vector<Vector>& vertices, const std::vector<Cara>& caras,
			 bool reflejante, float transparencia, float refraccion,
			 Color ambiente, Color difusa, Color especular)
	: Objeto(reflejante, transparencia, refraccion, ambiente, difusa, especular),
	  vertices(vertices), caras(caras), normal_ultimo_impacto(0, 1, 0) {}

Malla::Malla(const nlohmann::json& j)
	: Objeto(j),
	  vertices(j.at("vertices").get<std::vector<Vector>>()),
	  caras(j.at("caras").get<std::vector<Cara>>()) {}

float Malla::interseccion_mas_cercana(const Vector &p, const Vector &v) const {
	float t_min = std::numeric_limits<float>::infinity();
	bool hubo_impacto = false;
	Vector normal_temporal(0, 1, 0);

	for (const auto& cara : caras) {
		Vector p0 = vertices[cara.v0];
		Vector p1 = vertices[cara.v1];
		Vector p2 = vertices[cara.v2];
		Vector p3 = vertices[cara.v3];

		float tA = interseccion_triangulo(p, v, p0, p1, p2, normal_temporal);
		if (tA > 1e-4f && tA < t_min - 1e-4) {
			t_min = tA;
			normal_ultimo_impacto = normal_temporal;
			hubo_impacto = true;
		}

		float tB = interseccion_triangulo(p, v, p0, p2, p3, normal_temporal);
		if (tB > 1e-4f && tB < t_min - 1e-4) {
			t_min = tB;
			normal_ultimo_impacto = normal_temporal;
			hubo_impacto = true;
		}
	}

	return hubo_impacto ? t_min : -1.0f;
}

Vector Malla::normal_en_punto(const Vector &p) const {
	return normal_ultimo_impacto;
}
