#pragma once

#include <objeto.hpp>

class Cubo : public Objeto {
private:
	Vector centro;
	float largo, alto, ancho;
public:
	explicit Cubo(const nlohmann::json& j);

	virtual Interseccion interseccion_mas_cercana(const Vector &p, const Vector &v) const override;
};

