#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>

#include "renderer.hpp"

using namespace std;
using json = nlohmann::json;

int main() {
    ifstream archivo("escena.json");

    if (!archivo) {
    	cerr << "Archivo de escena inexistente: escena.json" << endl;
    	return 1;
    }

    try {
    	json j = json::parse(archivo);

		Escena escena(j);
		Renderer renderer(escena, j);

		renderer.dibujar(ModoRender::Completo)
			    .guardar("foto.png");

		renderer.dibujar(ModoRender::SoloReflexion)
			    .guardar("reflexion.png");

		renderer.dibujar(ModoRender::SoloTransparencia)
			    .guardar("transparencia.png");
	} catch (const json::type_error& e) {
	    cerr << "Tipo incorrecto en el JSON: " << e.what() << endl;
	    return 1;
	} catch (const json::out_of_range& e) {
	    cerr << "Falta un campo requerido: " << e.what() << endl;
	    return 1;
	} catch (const json::exception& e) {
	    cerr << "Error de JSON: " << e.what() << " (id " << e.id << ")" << endl;
	    return 1;
	}
}
