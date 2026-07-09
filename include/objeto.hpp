#pragma once

#include <nlohmann/json.hpp>

#include <utils/color.hpp>
#include <utils/material.hpp>
#include <utils/vector.hpp>

class Objeto {
private:
	Material material;
public:
	Objeto(const nlohmann::json& j);
 	
 	virtual ~Objeto() = default;

	// Devuelve propiedades del material del objeto
	const Material &get_material() const;

	// Devuelve el menor t > 0 tal que p + tv está en el objeto.
	// Devuelve negativo si no existe.
	virtual float interseccion_mas_cercana(const Vector &p, const Vector &v) const = 0;
	// Devuelve la normal del objeto en el punto p.
	virtual Vector normal_en_punto(const Vector &p) const = 0;
};
