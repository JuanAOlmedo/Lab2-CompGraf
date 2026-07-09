#pragma once

#include <utils/vector.hpp>
#include <utils/color.hpp>
#include <nlohmann/json.hpp>

class Objeto {
private:
	bool reflejante;
	float transparencia, refraccion;
	Color ambiente, difusa, especular;
public:
	Objeto(bool reflejante, float transparencia, float refraccion,
		   Color ambiente, Color difusa, Color especular);
	Objeto(const nlohmann::json& j);
 	
 	virtual ~Objeto() = default;

	// Devuelven si el objeto es reflejante y su transparencia
	bool get_reflejante() const;
	float get_transparencia() const;
	float get_refraccion() const;

	// Componentes de luz
	Color luz_ambiente() const; 
	Color luz_difusa() const;
	Color luz_especular() const;

	// Devuelve el menor t > 0 tal que p + tv está en el objeto.
	// Devuelve negativo si no existe.
	virtual float interseccion_mas_cercana(const Vector &p, const Vector &v) const = 0;
	// Devuelve la normal del objeto en el punto p.
	virtual Vector normal_en_punto(const Vector &p) const = 0;
};
