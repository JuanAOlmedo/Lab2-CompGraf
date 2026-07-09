#pragma once

#include <ostream>

#include "nlohmann/json.hpp"

class Vector {
public:
	float x, y, z;

	Vector();
	Vector(float x, float y, float z);

	Vector operator+(const Vector& w) const;
	Vector operator-(const Vector& w) const;
	Vector operator*(float a) const;
	Vector operator/(float a) const;
	Vector operator-() const;
	Vector& operator+=(const Vector& w);
	Vector& operator*=(float a);

	float operator*(const Vector& w) const;
	float producto_interno(const Vector& w) const;
	Vector producto_vectorial(const Vector& w) const;

	float get_norma() const;
	float get_norma_inf() const;
	float get_norma_2() const;
	Vector normal() const;

	float angulo(const Vector& w) const;
	Vector cambiar_angulo(const Vector& n, float theta) const;
	Vector reflexion(const Vector& w) const;

	friend Vector operator*(float a, const Vector& v) {
		return v * a;
	}

	friend std::ostream& operator<<(std::ostream& os, const Vector& v) {
		return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	}
};

void from_json(const nlohmann::json& j, Vector& vector);

