#pragma once

#include "escena.hpp"
#include <rayo.hpp>
#include "utils/imagen.hpp"
#include <utils/modo_render.hpp>

class Renderer {
private:
	const Escena &escena;
	int largo, alto, profundidad, celdas_aliasing;
	Vector posicion_camara, direccion_vista, up, direccion_barrido;

	Color color_pixel(ModoRender modo, float i, float j);
public:
	Renderer(const Escena &escena, const json &j);

	// Dibuja la escena y devuelve la imagen correspondiente
	Imagen dibujar(ModoRender modo);
};
