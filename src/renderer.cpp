#include <renderer.hpp>

using namespace std;
using namespace nlohmann;

Color Renderer::color_pixel(ModoRender modo, float i, float j) {
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
}

Imagen Renderer::dibujar(ModoRender modo) {
	vector<Color> pixeles;
	pixeles.reserve(largo * alto);

	// Crear largo * alto píxeles y calcular el color de cada uno.
	for (int i = 0; i < alto; i++) {
		for (int j = 0; j < largo; j++) {
			pixeles.push_back(color_pixel(modo, i, j));
		}
	}

	return Imagen(largo, alto, std::move(pixeles));
}
