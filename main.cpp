#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <limits>
#include <FreeImage.h>

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

	float get_norma_2() const {
		return x * x + y * y + z * z;
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

	Vector reflexion(const Vector& w) const {
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
	unsigned char r, g, b;
};

void guardar_render_a_png(const std::vector<Color>& pixel_buffer, int ancho, int alto, const char* nombre_archivo) {
	// 1. Inicializar la librería (obligatorio)
	FreeImage_Initialise();

	// 2. Crear el mapa de bits en FreeImage. 
	// Ojo: FreeImage por defecto usa el orden BGR/BGRA de 24 o 32 bits.
	FIBITMAP* imagen = FreeImage_Allocate(ancho, alto, 24); // 24 bits = 3 bytes por píxel (RGB)

	if (!imagen) {
		// Manejar error
		FreeImage_DeInitialise();
		return;
	}

	// 3. Pasar los píxeles de tu arreglo al formato de FreeImage
	// Nota: El Ray Tracing suele calcular de arriba-abajo, pero FreeImage lee de abajo-arriba (Y invertida).
	for (int y = 0; y < alto; y++) {
		// Conseguimos un puntero a la fila actual de la imagen de FreeImage
		BYTE* fila = FreeImage_GetScanLine(imagen, y);

		for (int x = 0; x < ancho; x++) {
			// En un Ray Tracer clásico, el origen (0,0) está arriba a la izquierda.
			// Para corregir la inversión vertical de FreeImage mapeamos la Y al revés:
			int indice_tu_buffer = (alto - 1 - y) * ancho + x;

			// FreeImage espera los bytes en orden B-G-R
			fila[x * 3 + 0] = pixel_buffer[indice_tu_buffer].b; // Blue
			fila[x * 3 + 1] = pixel_buffer[indice_tu_buffer].g; // Green
			fila[x * 3 + 2] = pixel_buffer[indice_tu_buffer].r; // Red
		}
	}

	// 4. Guardar la imagen en disco
	// El tercer parámetro son flags (PNG_DEFAULT es compresión estándar)
	FreeImage_Save(FIF_PNG, imagen, nombre_archivo, PNG_DEFAULT);

	// 5. Limpieza de memoria
	FreeImage_Unload(imagen);
	FreeImage_DeInitialise();
}

class Luz {
private:
	// Componentes de luz
	virtual Color luz_ambiente() = 0;
	virtual Color luz_difusa() = 0;
	virtual Color luz_especular() = 0;
};

class Objeto {
	bool reflejante;
	float transparencia;
	Color ambiente, difusa, especular;
public:
	Objeto(bool reflejante, float transparencia, Color ambiente, Color difusa, Color especular)
		: reflejante(reflejante), transparencia(transparencia),
		  ambiente(ambiente), difusa(difusa), especular(especular) {}

	// Devuelven si el objeto es reflejante y su transparencia
	bool get_reflejante() {
		return reflejante;
	}

	float get_transparencia() {
		return transparencia;
	}

	// Componentes de luz
	Color luz_ambiente() {
		return ambiente;
	}

	Color luz_difusa() {
		return difusa;
	}

	Color luz_especular() {
		return especular;
	}

	// Devuelve el menor t > 0 tal que p + tv está en el objeto.
	virtual float interseccion_mas_cercana(Vector p, Vector v) = 0;
	// Devuelve la normal del objeto en el punto p.
	virtual Vector normal_en_punto(Vector p) = 0;
};

class Esfera : public Objeto {
private:
	Vector centro;
	float radio;
public:
	Esfera(Vector centro, float radio,bool reflejante, float transparencia,
		   Color ambiente, Color difusa, Color especular)
		: centro(centro), radio(radio), Objeto(reflejante, transparencia, ambiente, difusa, especular) {}

	float interseccion_mas_cercana(Vector p, Vector v) {
		/*
		(p + tv - centro) * (p + tv - centro) = radio ^ 2
		p * p + p * tv - p * centro + p * tv + tv * tv - tv * centro - centro * p - centro * tv + centro * centro = radio ^ 2
		p * p + t^2(v * v) + centro * centro + 2t(p * v) - 2(p * centro) - 2t(centro * v) = radio ^ 2;
		(v * v)t^2 + (2(p * v) - 2(centro * v))t + p * p - centro * centro - 2(p * centro) - radio ^ 2;
		*/

		float c = p.get_norma_2() + centro.get_norma_2() - 2 * p.producto_interno(centro) - radio * radio,
			  b = 2 * p.producto_interno(v) - 2 * centro.producto_interno(v),
			  a = v.get_norma_2();

		if (b * b - 4 * a * c < 0)
			return -1; 

		float t1 = (-b - sqrt(b * b - 4 * a * c)) / (2 * a),
			  t2 = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);

		if (t1 < 0 && t2 < 0)
			return -1;

		if (t1 < 0)
			return t2;

		if (t2 < 0)
			return t1;

		return min(t1, t2);
	}

	Vector normal_en_punto(Vector p) {
		return (p - centro) / radio;
	}
};

/*
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
*/

class Escena {
private:
	vector<Objeto *> lista_objetos;
	vector<Luz *> lista_luces;
	Color color;
public:
	Escena(Color color_ambiente) : color(color_ambiente) {};
	Escena(string archivo);

	void agregar(Objeto *o) {
		lista_objetos.push_back(o);
	}

	void agregar(Luz *l) {
		lista_luces.push_back(l);
	}

	vector<Objeto *> objetos() {
		return lista_objetos;
	}

	vector<Luz *> luces() {
		return lista_luces;
	}

	Color color_ambiente() {
		return color;
	}
};

class Rayo {
private:
	Vector punto_inicial, direccion;

	Color sombra(Objeto *objeto, float t, Escena *escena, int profundidad) {
		Vector normal = objeto->normal_en_punto(punto_inicial + t * direccion);

		Color color = escena->color_ambiente();

		return objeto->luz_difusa();
	}
public:
	Rayo(Vector punto_inicial, Vector direccion)
		: punto_inicial(punto_inicial), direccion(direccion) {}

	Color color(Escena *escena, int profundidad) {
		int distancia_minima = numeric_limits<float>::infinity();
		Objeto *objeto_mas_cercano = nullptr;

		for (auto objeto : escena->objetos()) {
			float distancia = objeto->interseccion_mas_cercana(punto_inicial, direccion);

			if (distancia < distancia_minima && distancia > 0) {
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
	Imagen(Escena *escena, int largo, int alto, Vector posicion_camara)
		: escena(escena), largo(largo), alto(alto), posicion_camara(posicion_camara) {}

	// Dibuja la escena y devuelve los pixeles (el tamaño del vector es largo * alto)
	vector<Color> dibujar() {
		for (int i = 0; i < alto; i++) {
			for (int j = 0; j < largo; j++) {
				Vector direccion(i - alto / 2.0, j - largo / 2.0, 1);

				Rayo r(posicion_camara, direccion);
				pixeles.push_back(r.color(escena, 2));
			}
		}

		return pixeles;
	}

	// Guarda la imagen en un archivo
	void guardar(string archivo);
};

int main() {
	Color color_ambiente({0, 0, 0});
	Color color({255, 0, 0});
	Escena escena(color_ambiente);

	Vector posicion_esfera(0, 0, 2.1);
	Esfera e(posicion_esfera, 2, false, 1, color, color, color);
	escena.agregar(&e);

	int largo = 50, alto = 50;

	Vector posicion_camara(0, 0, 0);
	Imagen imagen(&escena, largo, alto, posicion_camara);

	guardar_render_a_png(imagen.dibujar(), largo, alto, "olmedo3.png");
}
