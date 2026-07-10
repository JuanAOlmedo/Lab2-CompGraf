#pragma once

#include <objeto.hpp>

class Esfera : public Objeto {
private:
	Vector centro;
	float radio;
public:
	explicit Esfera(const nlohmann::json& j);

	virtual Interseccion interseccion_mas_cercana(const Vector &p, const Vector &v) const override;
};

