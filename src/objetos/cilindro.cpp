#include <objetos/cilindro.hpp>

Cilindro::Cilindro(Vector c, float r, float h, bool reflectante, float transparencia,float refraccion,
		Color ambiente, Color difusa, Color especular)
	: Objeto(reflectante, transparencia, refraccion ,ambiente,difusa, especular),
	  centro(c), radio(r), altura(h) {}

Cilindro::Cilindro(const nlohmann::json& j)
	: Objeto(j),
	  centro(j.at("posicion").get<Vector>()),
	  radio(j.at("radio").get<float>()),
	  altura(j.at("altura").get<float>()) {}

float Cilindro::interseccion_mas_cercana(const Vector &p, const Vector &v) const {
	float t_min = std::numeric_limits<float>::infinity();
	bool hubo_interseccion = false;

	// Primero calculamos la intersección con el cuerpo lateral
	float a = v.x * v.x + v.z * v.z;
	float b = 2.0f * ((p.x - centro.x) * v.x + (p.z - centro.z) * v.z);
	float c = (p.x - centro.x) * (p.x - centro.x) + 
			  (p.z - centro.z) * (p.z - centro.z) - radio * radio;

	float det = b * b - 4.0f * a * c;

	if (det >= 0.0f) {
		float t1 = (-b - std::sqrt(det)) / (2.0f * a);
		float t2 = (-b + std::sqrt(det)) / (2.0f * a);

		// Revisamos ambas soluciones de la cuadrática
		for (float t : {t1, t2}) {
			if (t > 1e-4f && t < t_min && !hubo_interseccion) {
				// Calculamos la altura Y del punto de impacto
				float y_impacto = p.y + t * v.y;
				// Validamos si cae dentro de la altura del cilindro
				if (y_impacto >= centro.y && y_impacto <= centro.y + altura) {
					t_min = t;
					hubo_interseccion = true;
				}
			}
		}
	}

	// Ahora calculamos intersección con las tapas
	if (std::abs(v.y) > 1e-6f) {
		// Calcular intersecciones con los planos que contienen a las tapas
		float t_inf = (centro.y - p.y) / v.y;
		float t_sup = ((centro.y + altura) - p.y) / v.y;

		// Verificar si las intersecciones están dentro de las tapas y si son
		// mínimas.
		for (float t : {t_inf, t_sup}) {
			if (t > 1e-4f && t < t_min) {
				float x_impacto = p.x + t * v.x;
				float z_impacto = p.z + t * v.z;
				// Verificamos si el punto cae dentro del círculo de la tapa
				float dist_2 = (x_impacto - centro.x) * (x_impacto - centro.x) +
							   (z_impacto - centro.z) * (z_impacto - centro.z);
				if (dist_2 <= radio * radio) {
					t_min = t;
					hubo_interseccion = true;
				}
			}
		}
	}

	return hubo_interseccion ? t_min : -1.0f;
}

Vector Cilindro::normal_en_punto(const Vector &p) const {
	const float EPSILON = 1e-4;

	// Tapa superior:
	if (p.y >= (centro.y + altura) - EPSILON)
		return Vector(0.0f, 1.0f, 0.0f);
	// Tapa inferior:
	if (p.y <= centro.y + EPSILON)
		return Vector(0.0f, -1.0f, 0.0f);

	// Cuerpo lateral:
	// Le sacamos la componente en y para proyectar sobre el plano y = 0
	// (y que sea normal al cuerpo)
	Vector de_centro_a_p = p - centro;
	// Dividimos entre el radio para normalizar
	return Vector(de_centro_a_p.x / radio, 0.0f, de_centro_a_p.z / radio);
}
