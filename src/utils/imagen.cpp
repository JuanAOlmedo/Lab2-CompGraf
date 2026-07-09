#include <utils/imagen.hpp>
#include <iostream>

Imagen::Imagen(int largo, int alto, std::vector<Color> pixeles)
	: largo(largo), alto(alto), pixeles(std::move(pixeles)) {}

void Imagen::guardar(const std::string &archivo) {
	// Inicializar librería
	FreeImage_Initialise();
	FIBITMAP *imagen = FreeImage_Allocate(largo, alto, 24);

	if (!imagen) {
		FreeImage_DeInitialise();
		std::cerr << "Error de FreeImage" << std::endl; 
		exit(1);
	}

	for (int i = 0; i < alto; i++) {
		// Conseguimos un puntero a la fila actual de la imagen de FreeImage
		BYTE *fila = FreeImage_GetScanLine(imagen, i);

		for (int j = 0; j < largo; j++) {
			// FreeImage invierte la imagen. Damos vuelta el componente i.
			int indice = (alto - 1 - i) * largo + j;

			// FreeImage espera los bytes en orden B-G-R
			fila[j * 3 + 0] = static_cast<unsigned char>(pixeles[indice].b);
			fila[j * 3 + 1] = static_cast<unsigned char>(pixeles[indice].g);
			fila[j * 3 + 2] = static_cast<unsigned char>(pixeles[indice].r);
		}
	}

	FreeImage_Save(FIF_PNG, imagen, archivo.data(), PNG_DEFAULT);

	FreeImage_Unload(imagen);
	FreeImage_DeInitialise();
}
