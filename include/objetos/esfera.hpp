#pragma once

#include <objeto.hpp>

class Esfera : public Objeto {
private:
	Vector centro;
	float radio;
public:
	explicit Esfera(const nlohmann::json& j);

	float interseccion_mas_cercana(const Vector &p, const Vector &v) const override;
	Vector normal_en_punto(const Vector &p) const override;
};

