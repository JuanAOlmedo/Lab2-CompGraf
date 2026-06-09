#include <vector>
#include <string>

using namespace std;

struct Color {
	float r, g, b;
};

class Luz {
private:
	// Componentes de luz
	virtual Color luz_ambiente() = 0;
	virtual Color luz_difusa() = 0;
	virtual Color luz_especular() = 0;
}

class Objeto {
public:
	// Devuelven si el objeto es reflejante y su transparencia
	virtual bool reflejante() = 0;
	virtual float transparencia() = 0;

	// Componentes de luz
	virtual Color luz_ambiente() = 0;
	virtual Color luz_difusa() = 0;
	virtual Color luz_especular() = 0;

	// Devuelve el menor t > 0 tal que p + tv está en el objeto.
	virtual float interseccion_mas_cercana(Punto p, Vector v) = 0;
	// Devuelve la normal del objeto en el punto p.
	virtual Vector normal_en_punto(Punto p) = 0;
};

class Esfera : public Objeto {

};

class Cilindro : public Objeto {

};

class Malla : public Objeto {

};

class Superficie : public Objeto {

};

class Escena {
private:
	vector<Objeto *> lista_objetos;
	vector<Luz *> lista_luces;
public:
	void agregar(Objeto *o);
	void agregar(Luz *l);

	vector<Objeto *> objetos();
	vector<Luz *> luces();
};

class Imagen {
private:
	Escena *escena;
	int largo, alto;
	vector<Color> pixeles;
public:
	Imagen(Escena *escena, int largo, int alto);
	// Dibuja la escena y devuelve los pixeles (el tamaño del vector es largo * alto)
	vector<Color> dibujar();
	// Guarda la imagen en un archivo
	void guardar(string archivo);
};

int main() {

}
