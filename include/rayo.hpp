#pragma once

#include <objeto.hpp>
#include <escena.hpp>
#include <utils/modo_render.hpp>

class Rayo {
private:
	const float EPSILON = 3e-4;
	const float INDICE_REFRACCION_AIRE = 1;
	const float COEFICIENTE_ESPECULAR = 100;

	const Vector punto_inicial, direccion;
	const Escena &escena;
	const Objeto *adentro; // Estamos adentro de este objeto

	int profundidad;

	// Calcula qué tan tapado está el rayo por objetos.
	// Por ejemplo, si el rayo interseca algún objeto opaco en distancia
	// menor a distancia_maxima, devuelve negro. Si interseca un objeto transparente,
	// devuelve el color del objeto.
	Color oclusion(float distancia_maxima) const;

	Color reflexion(const Vector &punto, const Vector &normal) const;

	// Aplica la ley de Snell a un vector v que pasa de un material con
	// coeficiente de refracción nabla1 a otro con coeficiente nabla2,
	// colisionando en un punto con normal n.
	// Si ocurre refracción interna total, no devuelve nada.
	static optional<Vector> aplicar_snell(const Vector &v, const Vector &n, 
								          float nabla1, float nabla2);

	Color transparencia(const Objeto *objeto, const Vector &punto, const Vector &normal) const;

	// Devuelve el color que debería tener el rayo, sabiendo que intersecó
	// el objeto especificado a distancia d del punto inicial.
	Color sombra(const Objeto *objeto, float d) const;

	// Devuelve el objeto más cercano al rayo y la distancia a él.
	// Si no interseca ningún objeto, devuelve {nullptr, infinity}.
	pair<const Objeto *, float> objeto_mas_cercano() const;

	// Especifica adentro de qué objeto se está.
	void adentro_de(const Objeto *o);
public:
	Rayo(const Vector punto_inicial, const Vector direccion, const Escena &escena, int profundidad);
	
	Color trazar(ModoRender modo) const;
};
