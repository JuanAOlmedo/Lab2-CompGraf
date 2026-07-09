#pragma once

#include <objeto.hpp>

class Esfera : public Objeto {
private:
	Vector centro;
	float radio;
public:
	Esfera(Vector centro, float radio, bool reflejante, float transparencia, float refraccion,
		   Color ambiente, Color difusa, Color especular);
	explicit Esfera(const nlohmann::json& j);

	float interseccion_mas_cercana(const Vector &p, const Vector &v) const override;
	Vector normal_en_punto(const Vector &p) const override;
};

