#pragma once

#include <objeto.hpp>

class Cilindro : public Objeto {
private:
	Vector centro;
	float radio;
	float altura;
public:
	explicit Cilindro(const nlohmann::json& j);

	float interseccion_mas_cercana(const Vector &p, const Vector &v) const override;
	Vector normal_en_punto(const Vector &p) const override;
};
