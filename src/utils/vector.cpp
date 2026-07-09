#include <cmath>
#include <algorithm>
#include "utils/vector.hpp"
#include "nlohmann/json.hpp"

Vector::Vector() : x(0), y(0), z(0) {}

Vector::Vector(float x, float y, float z) : x(x), y(y), z(z) {}

Vector Vector::operator+(const Vector& w) const {
	return Vector(
		x + w.x,
		y + w.y,
		z + w.z
	);
}

Vector& Vector::operator+=(const Vector& w) {
	x += w.x;
	y += w.y;
	z += w.z;

	return *this;
}

Vector Vector::operator-(const Vector& w) const {
	return Vector(
		x - w.x,
		y - w.y,
		z - w.z
	);
}

Vector Vector::operator*(float a) const {
	return Vector(
		x * a,
		y * a,
		z * a
	);
}	

float Vector::operator*(const Vector& w) const {
	return x * w.x + y * w.y + z * w.z;
}

Vector& Vector::operator*=(float a) {
	x *= a;
	y *= a;
	z *= a;

	return *this;
}

Vector Vector::operator/(float a) const {
	assert(a != 0.0f);

	return Vector(
		x / a,
		y / a,
		z / a
	);
}

Vector Vector::operator-() const {
	return Vector(
		-x,
		-y,
		-z
	);
}

float Vector::producto_interno(const Vector& w) const {
	return x * w.x + y * w.y + z * w.z;
}

Vector Vector::producto_vectorial(const Vector& w) const {
	return Vector(
		y * w.z - z * w.y,
		z * w.x - x * w.z,
		x * w.y - y * w.x
	);
}

float Vector::get_norma() const {
	return std::sqrt(x * x + y * y + z * z);
}

float Vector::get_norma_inf() const {
	return fmax(fmax(x, y), z);
}

float Vector::get_norma_2() const {
	return x * x + y * y + z * z;
}

Vector Vector::normal() const {
	float norm = get_norma();

	if (norm <= std::numeric_limits<float>::epsilon())
		return *this;

	return *this / norm;
}

float Vector::angulo(const Vector& w) const {
	float denom = get_norma() * w.get_norma();

	if (denom <= std::numeric_limits<float>::epsilon())
		return 0.0f;

	// Forzar que dot_product(w) / denom esté entre -1 y 1 para evitar
	// error al aplicar acos()
	float c = std::clamp(producto_interno(w) / denom, -1.0f, 1.0f);

	return std::acos(c);
}

Vector Vector::cambiar_angulo(const Vector& n, float theta) const {
	// n tiene que tener norma 1
	// Componente de direccion perpendicular a normal, dentro del plano
	Vector proj = (n * *this) * n;
	Vector perp = *this - proj;

	float norma_perp = perp.get_norma();
	if (norma_perp < 1e-9) {
		// direccion es paralelo (o antiparalelo) a normal: el plano no está
		// determinado, elegimos un perpendicular arbitrario
		Vector ref = (std::abs(n.x) < 0.9) ? Vector(1,0,0) : Vector(0,1,0);
		perp = n.producto_vectorial(ref);
		norma_perp = perp.get_norma();
	}
	perp = perp / norma_perp;

	// Nueva direccion unitaria a ángulo theta de normal, en el mismo plano
	return n * std::cos(theta) + perp * std::sin(theta);
}

Vector Vector::reflexion(const Vector& w) const {
	auto w_normalizado = w.normal();

	return *this - w_normalizado * 2 * producto_interno(w_normalizado);
}


void from_json(const nlohmann::json& j, Vector& vector) {
    if (!j.is_array() || j.size() != 3) {
        throw nlohmann::json::type_error::create(
            302,
            "Los vectores son arreglos de tres elementos",
            &j
        );
    }

    vector.x = j.at(0).get<float>();
    vector.y = j.at(1).get<float>();
    vector.z = j.at(2).get<float>();
}
