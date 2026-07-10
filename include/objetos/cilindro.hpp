#pragma once

#include <objeto.hpp>

class Cilindro : public Objeto {
private:
	Vector centro;
	float radio;
	float altura;
public:
	explicit Cilindro(const nlohmann::json& j);

	virtual Interseccion interseccion_mas_cercana(const Vector &p, const Vector &v) const override;
};
