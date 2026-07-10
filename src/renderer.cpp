#include <renderer.hpp>
#include <rayo.hpp>
#include <iostream>
#include <thread>
#include <functional>
#include <pthread.h>

using namespace std;
using namespace nlohmann;

Color Renderer::color_pixel(ModoRender modo, float i, float j) const {
	Color color({0, 0, 0});

	// Implementación de antialiasing:
	// Se crean (celdas_aliasing * celdas_aliasing) celdas y se traza
	// un rayo por cada una de ellas.
	for (int n = 0; n < celdas_aliasing; n++) {
		for (int m = 0; m < celdas_aliasing; m++) {
			// El pixel está ubicado en el punto posicion_camara + direccion.
			// Calculamos direccion separando en tres componentes perpendiculares:
			// direccion_vista, direccion_barrido y up.
			// Dividimos las últimas dos componentes entre alto para normalizar el tamaño
			// de píxeles.
			Vector direccion =
				direccion_vista
				+ direccion_barrido * (j + (float) n / celdas_aliasing - largo / 2.0) / alto
				+ up * (alto / 2.0 - i - (float) m / celdas_aliasing) / alto;

			Rayo r(posicion_camara + direccion, direccion.normal(), escena, profundidad);
			color += r.trazar(modo) * (1.0 / celdas_aliasing / celdas_aliasing);
		}
	}

	return color;
}

Renderer::Renderer(const Escena &escena, const json &j)
	: escena(escena),
	  largo(j.at("largo_imagen").get<int>()),
	  alto(j.at("alto_imagen").get<int>()),
	  profundidad(j.at("recursion").get<int>()),
	  celdas_aliasing(j.at("celdas_para_aliasing").get<int>()),
	  cant_threads(j.at("cantidad_de_hilos").get<int>()),
	  posicion_camara(j.at("posicion_camara").get<Vector>()),
	  direccion_vista(j.at("direccion_vista").get<Vector>()),
	  up(j.at("direccion_arriba").get<Vector>().normal()),
	  direccion_barrido(up.producto_vectorial(direccion_vista.normal())) {
	if (largo <= 0 || alto <= 0) {
		cerr << "El largo y alto de la imagen tiene que ser positivo" << endl;
		exit(1);
	}
	if (profundidad <= 0) {
		cerr << "La profundidad de recursión tiene que ser positiva" << endl;
		exit(1);
	}
	if (celdas_aliasing <= 0) {
		cerr << "La cantidad de celdas para aliasing tiene que ser positiva" << endl;
		exit(1);
	}
	if (cant_threads <= 0) {
		cerr << "La cantidad de hilos tiene que ser positiva" << endl;
		exit(1);
	}
}

void Renderer::dibujar_filas(ModoRender modo, vector<Color> &pixeles, int primera, int salto) const {
	// Crear largo * alto píxeles y calcular el color de cada uno.
	for (int i = primera; i < alto; i += salto) {
		for (int j = 0; j < largo; j++) {
			pixeles[i * largo + j] = color_pixel(modo, i, j);
		}
	}
}

Imagen Renderer::dibujar(ModoRender modo) const {
	vector<Color> pixeles;
	pixeles.resize(largo * alto);
	vector<thread> threads;
	threads.resize(cant_threads - 1);

	for (int i = 0; i < cant_threads - 1; i++) {
		threads[i] = thread(&Renderer::dibujar_filas, this, modo, std::ref(pixeles), i, cant_threads);
	}
	dibujar_filas(modo, pixeles, cant_threads - 1, cant_threads);

	for (auto &thread : threads) {
		if (thread.joinable())
			thread.join();
	}

	return Imagen(largo, alto, std::move(pixeles));
}
