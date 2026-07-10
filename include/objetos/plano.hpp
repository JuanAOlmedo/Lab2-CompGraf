#pragma once

#include <objeto.hpp>


class Plano : public Objeto{
private:
	Vector punto_plano;
	Vector normal_plano;
public:
	explicit Plano(const nlohmann::json& j);

	virtual Interseccion interseccion_mas_cercana(const Vector &p, const Vector &v) const override;
};
