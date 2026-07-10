#pragma once

#include <vector>

#include <objeto.hpp>
#include <utils/volumen_acotante.hpp>

struct Cara;

class Malla : public Objeto {
private:
	std::vector<Vector> vertices;
	std::vector<Cara> caras;
	VolumenAcotante vol;

    static float interseccion_triangulo(const Vector& p, const Vector& v,
    		const Vector& v0, const Vector& v1, const Vector& v2, Vector& normal_out);

	Vector min_coordenadas() const;
	Vector max_coordenadas() const;
public:
	explicit Malla(const nlohmann::json& j);

	virtual Interseccion interseccion_mas_cercana(const Vector &p, const Vector &v) const override;
};
