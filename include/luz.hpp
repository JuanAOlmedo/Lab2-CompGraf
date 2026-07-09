#pragma once

#include <utils/vector.hpp>
#include <utils/color.hpp>
#include <nlohmann/json.hpp>

class Luz {
private:
	Vector posicion;
	Color difusa, especular;
public:
	Luz(Vector posicion, Color difusa, Color especular);
	explicit Luz(const nlohmann::json& j);

	Vector get_posicion() const;
	Color luz_difusa() const;
	Color luz_especular() const;
};

