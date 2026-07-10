#pragma once

#include <utils/vector.hpp>
#include <algorithm>

// Representa geométricamente a un cubo.
// Usado para envolver mallas poligonales y evitar verificar intersecciones
// con todos los triángulos en algunos casos.
class VolumenAcotante {
private:
	Vector vertice1, vertice2;

public:
	// El vértice 1 es el vértice de coordenadas menores del cubo,
	// el 2 el de coordenadas mayores
	VolumenAcotante(Vector vertice1, Vector vertice2)
		: vertice1(vertice1), vertice2(vertice2) {};

	// Devuelve true si y solo si la recta p + t * v
	// interseca al cubo.
	bool interseccion(Vector p, Vector v) const {
		float t1x = (vertice1.x - p.x) / v.x;
		float t2x = (vertice2.x - p.x) / v.x;

		float tminx = std::min(t1x, t2x);
		float tmaxx = std::max(t1x, t2x);

		float t1y = (vertice1.y - p.y) / v.y;
		float t2y = (vertice2.y - p.y) / v.y;

		float tminy = std::min(t1y, t2y);
		float tmaxy = std::max(t1y, t2y);

		float t1z = (vertice1.z - p.z) / v.z;
		float t2z = (vertice2.z - p.z) / v.z;

		float tminz = std::min(t1z, t2z);
		float tmaxz = std::max(t1z, t2z);

		float tmin = std::max(std::max(tminx, tminy), tminz);
		float tmax = std::min(std::min(tmaxx, tmaxy), tmaxz);

		return tmin <= tmax && tmax >= 0;
	}
};
