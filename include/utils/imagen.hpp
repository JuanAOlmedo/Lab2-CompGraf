#pragma once

#include <vector>
#include <string>

#include <utils/color.hpp>
#include <FreeImage.h>

class Imagen {
	int largo, alto;
	const std::vector<Color> pixeles;

public:
	Imagen(int largo, int alto, std::vector<Color> pixeles);

	// Guarda la imagen en un archivo
	void guardar(const std::string &archivo);
};
