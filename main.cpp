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
	virtual Vector normal_en_punto(Vector p) = 0;
};

class Esfera : public Objeto {
private:
	Vector centro;
	float radio;
public:
	Esfera(Vector c, float r) : centro(c), radio(r) {}
	Vector normal_en_punto(Vector p) {
		return (p - centro) / radio;
	}
};

class Cilindro : public Objeto {
private:
	Vector centro; // Un punto por el que pasa el eje central del cilindro
	float radio;
	float altura;
public:
	// Constructor para inicializar los valores
	Cilindro(Vector c, float r, float h) : centro(c), radio(r), altura(h) {}

	class Cilindro : public Objeto {
	private:
		Vector centro; // Centro de la base inferior del cilindro
		float radio;
		float altura;  // Altura total del cilindro

	public:
		Cilindro(Vector c, float r, float h) : centro(c), radio(r), altura(h) {}

		Vector normal_en_punto(Vector p) {
			// Un margen de error pequeño para las comparaciones de punto flotante
			const float EPSILON = 0.0001f;

			// 1. Verificar si el punto está en la tapa superior
			// La altura en Y de la tapa superior es: centro.y + altura
			if (p.y >= (centro.y + altura) - EPSILON) {
				return Vector(0.0f, 1.0f, 0.0f); // Normal hacia arriba
			}

			// 2. Verificar si el punto está en la tapa inferior
			// La altura en Y de la tapa inferior es: centro.y
			if (p.y <= centro.y + EPSILON) {
				return Vector(0.0f, -1.0f, 0.0f); // Normal hacia abajo
			}

			// 3. Si no está en las tapas, está en el cuerpo lateral
			Vector de_centro_a_p = p - centro;
			de_centro_a_p.y = 0.0f; // Ignoramos la altura para la pared lateral

			return de_centro_a_p / radio; // Vector normalizado hacia afuera
		}
	};
};

class Malla : public Objeto {
public:
	normal_en_punto(Vector	 p) {
		// Calcular la normal usando los vértices de la malla
		return Vector(0, 0, 1); // Placeholder, replace with actual normal calculation
	}
};

class Superficie : public Objeto {
public:
	normal_en_punto(Vector p) {
		// Calcular la normal usando la función de la superficie
		return Vector(0, 0, 1); // Placeholder, replace with actual normal calculation
	}
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
