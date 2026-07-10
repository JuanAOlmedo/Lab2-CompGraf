#pragma once

#include <utils/vector.hpp>
class Objeto;

struct Interseccion {
	const Objeto *objeto;
	float d;
	Vector normal;
};
