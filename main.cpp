#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <limits>

using namespace std;

class Vector {
private:
	float x, y, z;
public:
	Vector(float x, float y, float z)
		: x(x), y(y), z(z) {}

	float get_x() const {
		return x;
	}

	float get_y() const {
		return y;
	}

	float get_z() const {
		return z;
	}

	Vector operator+(const Vector& w) const {
		return Vector(
			x + w.x,
			y + w.y,
			z + w.z
		);
	}

	Vector& operator+=(const Vector& w) {
	    x += w.x;
	    y += w.y;
	    z += w.z;

		return *this;
	}

	Vector operator-(const Vector& w) const {
		return Vector(
			x - w.x,
			y - w.y,
			z - w.z
		);
	}

	Vector operator*(float a) const {
		return Vector(
			x * a,
			y * a,
			z * a
		);
	}

	Vector& operator*=(float a) {
	    x *= a;
	    y *= a;
	    z *= a;

		return *this;
	}

	Vector operator/(float a) const {
		assert(a != 0.0f);

		return Vector(
			x / a,
			y / a,
			z / a
		);
	}

	Vector operator-() const {
		return Vector(
			-x,
			-y,
			-z
		);
	}

	float producto_interno(const Vector& w) const {
		return x * w.x + y * w.y + z * w.z;
	}

	Vector producto_vectorial(const Vector& w) const {
		return Vector(
			y * w.z - z * w.y,
			z * w.x - x * w.z,
			x * w.y - y * w.x
		);
	}

	float get_norma() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	Vector normal() const {
		float norm = get_norma();

		if (norm <= std::numeric_limits<float>::epsilon())
    		return *this;

		return *this / norm;
	}

	float angulo(const Vector& w) const {
	    float denom = get_norma() * w.get_norma();

	    if (denom <= std::numeric_limits<float>::epsilon())
    		return 0.0f;

	    // Forzar que dot_product(w) / denom esté entre -1 y 1 para evitar
	    // error al aplicar acos()
	    float c = std::clamp(producto_interno(w) / denom, -1.0f, 1.0f);

	    return std::acos(c);
	}

	constexpr Vector reflexion(const Vector& w) const {
		auto w_normalizado = w.normal();

		return *this - w_normalizado * 2 * producto_interno(w_normalizado);
	}

	friend Vector operator*(float a, const Vector& v) {
	    return v * a;
	}

	friend std::ostream& operator<<(std::ostream& os, const Vector& v) {
	    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	}
};

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
	virtual float interseccion_mas_cercana(Vector p, Vector v) = 0;
	// Devuelve la normal del objeto en el punto p.
	virtual Vector normal_en_punto(Vector p) = 0;
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
	Escena();
	Escena(string archivo);

	void agregar(Objeto *o);
	void agregar(Luz *l);

	vector<Objeto *> objetos();
	vector<Luz *> luces();
	Color color_ambiente();
};

class Rayo {
private:
	Vector punto_inicial, direccion;

	Color sombra(Objeto *objeto, float t, Escena *escena, int profundidad) {
		Vector normal = objeto->normal_en_punto(punto_inicial + distancia_minima * direccion);

		Color color = escena->color_ambiente();

		return color;
	}
public:
	Rayo(Vector punto_inicial, Vector direccion)
		: punto_inicial(punto_inicial), direccion(direccion) {}

	Color color(Escena *escena, int profundidad) {
		int distancia_minima = Inf;
		Objeto *objeto_mas_cercano = nullptr;

		for (auto objeto : escena->objetos()) {
			float distancia = objeto->interseccion_mas_cercana(punto_inicial, direccion);

			if (distancia < distancia_minima) {
				objeto_mas_cercano = objeto;
				distancia_minima = distancia;
			}
		}

		if (objeto_mas_cercano != nullptr) {
			return sombra(objeto_mas_cercano, distancia_minima, escena, profundidad);
		} else {
			return escena->color_ambiente();
		}
	}
};

class Imagen {
private:
	Escena *escena;
	int largo, alto;
	Vector posicion_camara; // Sugerencia de Pilo: Con tres vectores

	vector<Color> pixeles;
public:
	Imagen(Escena *escena, int largo, int alto)
		: escena(escena), largo(largo), alto(alto) {}

	// Dibuja la escena y devuelve los pixeles (el tamaño del vector es largo * alto)
	vector<Color> dibujar() {
		for (int i = 0; i < alto; i++) {
			for (int j = 0; j < largo; j++) {
				Vector direccion(i - alto / 2.0, j - largo / 2.0, 1);

				Rayo r(posicion_camara, direccion);
				pixeles[i * largo + j] = r.color(escena, 2);
			}
		}
	}

	// Guarda la imagen en un archivo
	void guardar(string archivo);
};

int main() {

}
