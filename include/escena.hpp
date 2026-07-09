#pragma once

#include <utils/vector.hpp>
#include <utils/color.hpp>
#include <objeto.hpp>
#include <luz.hpp>
#include <nlohmann/json.hpp>
#include <vector>

using namespace std;
using namespace nlohmann;

class Escena {
private:
	vector<const Objeto *> lista_objetos;
	vector<const Luz *> lista_luces;
	Color color, ambiente;
public:
	Escena(Color color_fondo, Color luz_ambiente);
		
	explicit Escena(json j);

	~Escena();

	void agregar(const Objeto *o);
	void agregar(const Luz *l);

	const vector<const Objeto *> &objetos() const;
	const vector<const Luz *> &luces() const;

	Color color_fondo() const;
	Color luz_ambiente() const;
};
