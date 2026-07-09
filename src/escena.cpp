#include <escena.hpp>
#include <objetos/esfera.hpp>
#include <objetos/plano.hpp>
#include <objetos/cilindro.hpp>
#include <objetos/malla.hpp>

Escena::Escena(Color color_fondo, Color luz_ambiente)
	: color(color_fondo), ambiente(luz_ambiente) {};
	
Escena::Escena(json j)
	: color(j.at("color_fondo").get<Color>()),
	  ambiente(j.at("luz_ambiente").get<Color>()) {
	for (const auto& esfera : j.at("objetos")["esferas"]) {
		Objeto *o = new Esfera(esfera);
		lista_objetos.push_back(o);
	}

	for (const auto& cilindro : j.at("objetos")["cilindros"]) {
		Objeto *o = new Cilindro(cilindro);
		lista_objetos.push_back(o);
	}

	for (const auto& plano : j.at("objetos")["planos"]) {
		Objeto *o = new Plano(plano);
		lista_objetos.push_back(o);
	}

	for (const auto& malla : j.at("objetos")["mallas"]) {
		Objeto *o = new Malla(malla);
		lista_objetos.push_back(o);
	}

	for (const auto& luz : j["luces"]) {
		Luz *l = new Luz(luz);
		lista_luces.push_back(l);
	}
}

Escena::~Escena() {
	for (auto o : lista_objetos)
		delete o;

	for (auto l : lista_luces)
		delete l;
}

void Escena::agregar(const Objeto *o) {
	lista_objetos.push_back(o);
}

void Escena::agregar(const Luz *l) {
	lista_luces.push_back(l);
}

const vector<const Objeto *> &Escena::objetos() const {
	return lista_objetos;
}

const vector<const Luz *> &Escena::luces() const {
	return lista_luces;
}

Color Escena::color_fondo() const {
	return color;
}

Color Escena::luz_ambiente() const {
	return ambiente;
}
