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

	float operator*(const Vector& w) const {
		return x * w.x + y * w.y + z * w.z;
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

Color multiplicacion(Color c1, Color c2) {
	return {
        static_cast<unsigned char>(min(c1.r * c2.r / 255.0, 255.0)),
        static_cast<unsigned char>(min(c1.g * c2.g / 255.0, 255.0)),
        static_cast<unsigned char>(min(c1.b * c2.b / 255.0, 255.0))
	};
}

Color multiplicacion(Color c1, float x) {
	return {
		static_cast<unsigned char>(min(c1.r * x, 255.0f)),
		static_cast<unsigned char>(min(c1.g * x, 255.0f)),
		static_cast<unsigned char>(min(c1.b * x, 255.0f))
	};
}

Color suma(Color c1, Color c2) {
	return {
		static_cast<unsigned char>(min(c1.r + c2.r, 255)),
		static_cast<unsigned char>(min(c1.g + c2.g, 255)),
		static_cast<unsigned char>(min(c1.b + c2.b, 255))
	};
}

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
	Vector posicion;
	Color difusa, especular;
public:
	Luz(Vector posicion, Color difusa, Color especular)
		: posicion(posicion), difusa(difusa), especular(especular) {}

	Vector get_posicion() {
		return posicion;
	}

	Color luz_difusa() {
		return difusa;
	}

	Color luz_especular() {
		return especular;
	}
};

class Objeto {
private:
	bool reflejante;
	float transparencia, refraccion;
	Color ambiente, difusa, especular;
public:
	Objeto(bool reflejante, float transparencia, float refraccion, Color ambiente, Color difusa, Color especular)
		: reflejante(reflejante), transparencia(transparencia), refraccion(refraccion),
		  ambiente(ambiente), difusa(difusa), especular(especular) {}

	// Devuelven si el objeto es reflejante y su transparencia
	bool get_reflejante() const {
		return reflejante;
	}

	float get_transparencia() const {
		return transparencia;
	}

	float get_refraccion() const {
		return refraccion;
	}

	// Componentes de luz
	Color luz_ambiente() const {
		return ambiente;
	}

	Color luz_difusa() const {
		return difusa;
	}

	Color luz_especular() const {
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
	Esfera(Vector centro, float radio, bool reflejante, float transparencia, float refraccion,
		   Color ambiente, Color difusa, Color especular)
		: centro(centro), radio(radio), Objeto(reflejante, transparencia, refraccion, ambiente, difusa, especular) {}

	// Devuelve el menor t > 0 tal que p + tv está en el objeto.
	float interseccion_mas_cercana(Vector p, Vector v) {
		/*
		(p + tv - centro) * (p + tv - centro) = radio ^ 2
		p * p + p * tv - p * centro + p * tv + tv * tv - tv * centro - centro * p - centro * tv + centro * centro = radio ^ 2
		p * p + t^2(v * v) + centro * centro + 2t(p * v) - 2(p * centro) - 2t(centro * v) = radio ^ 2;
		(v * v)t^2 + (2(p * v) - 2(centro * v))t + p * p + centro * centro - 2(p * centro) - radio ^ 2 = 0;
		*/
		float c = p.get_norma_2() + centro.get_norma_2() - 2 * p.producto_interno(centro) - radio * radio,
			  b = 2 * p.producto_interno(v) - 2 * centro.producto_interno(v),
			  a = v.get_norma_2();

		float det = b * b - 4 * a * c;
		if (det < 0)
			return -1; 

		float t1 = (-b - sqrt(det)) / (2 * a),
			  t2 = (-b + sqrt(det)) / (2 * a);

		if (t1 > 1e-2)
			return t1 - 1;

		if (t2 > 1e-2)
			return t2 - 1;

		return -1;
	}

	Vector normal_en_punto(Vector p) {
		return (p - centro) / radio;
	}
};

class Plano : public Objeto{
private:
	Vector punto_plano;
	Vector normal_plano;
public:
	Plano(Vector punto, Vector normal, bool reflejante, bool transparencia,
	Color ambiente, Color difusa, Color especular) : punto_plano(punto), normal_plano(normal.normal()), 
          Objeto(reflejante, transparencia, 1, ambiente, difusa, especular) {}
	
	float interseccion_mas_cercana(Vector p, Vector v){
		float denominador = v.producto_interno(normal_plano);

		// Si el denominador es casi cero, el rayo es paralelo al plano
        if (std::abs(denominador) < 1e-6f) {
            return -1.0f;
        }

		//fórmula: t = ((q - p) . n) / (v . n)
        float t = (punto_plano - p).producto_interno(normal_plano) / denominador;

		// Si t es positivo, la intersección ocurrió adelante de la cámara
        if (t > 1e-6f) {
            return t;
        }

        return -1.0f;
	}

	Vector normal_en_punto(Vector p) override {
        // La normal de un plano es constante en toda su superficie
        return normal_plano;
    }
};


class Cilindro : public Objeto {
private:
	Vector centro;
	float radio;
	float altura;
public:
	Cilindro(Vector c, float r, float h, bool reflectante, float transparencia,
			Color ambiente, Color difusa, Color especular)
			: centro(c), radio(r), altura(h), Objeto(reflectante, transparencia,ambiente,difusa, especular){}
	float interseccion_mas_cercana(Vector p, Vector v) override {
		float t_min = std::numeric_limits<float>::infinity();
		bool hubo_interseccion = false;

		// --- 1. INTERSECCIÓN CON EL CUERPO LATERAL ---
        float a = v.get_x() * v.get_x() + v.get_z() * v.get_z();
        float b = 2.0f * ((p.get_x() - centro.get_x()) * v.get_x() + (p.get_z() - centro.get_z()) * v.get_z());
        float c = (p.get_x() - centro.get_x()) * (p.get_x() - centro.get_x()) + 
                  (p.get_z() - centro.get_z()) * (p.get_z() - centro.get_z()) - radio * radio;

        float det = b * b - 4.0f * a * c;

        if (det >= 0.0f) {
            float t1 = (-b - std::sqrt(det)) / (2.0f * a);
            float t2 = (-b + std::sqrt(det)) / (2.0f * a);

            // Revisamos ambas soluciones de la cuadrática
            for (float t : {t1, t2}) {
                if (t > 1e-4f && t < t_min) {
                    // Calculamos la altura Y del punto de impacto
                    float y_impacto = p.get_y() + t * v.get_y();
                    // Validamos si cae dentro de la altura del cilindro
                    if (y_impacto >= centro.get_y() && y_impacto <= centro.get_y() + altura) {
                        t_min = t;
                        hubo_interseccion = true;
                    }
                }
            }
        }

        // --- 2. INTERSECCIÓN CON LAS TAPAS (PLANOS LIMITADOS) ---
        // Tapa Inferior (Plano en Y = centro.y)
        if (std::abs(v.get_y()) > 1e-6f) {
            float t_inf = (centro.get_y() - p.get_y()) / v.get_y();
            if (t_inf > 1e-4f && t_inf < t_min) {
                float x_impacto = p.get_x() + t_inf * v.get_x();
                float z_impacto = p.get_z() + t_inf * v.get_z();
                // Verificamos si el punto cae dentro del círculo de la tapa
                float dist_2 = (x_impacto - centro.get_x()) * (x_impacto - centro.get_x()) +
                               (z_impacto - centro.get_z()) * (z_impacto - centro.get_z());
                if (dist_2 <= radio * radio) {
                    t_min = t_inf;
                    hubo_interseccion = true;
                }
            }

            // Tapa Superior (Plano en Y = centro.y + altura)
            float t_sup = ((centro.get_y() + altura) - p.get_y()) / v.get_y();
            if (t_sup > 1e-4f && t_sup < t_min) {
                float x_impacto = p.get_x() + t_sup * v.get_x();
                float z_impacto = p.get_z() + t_sup * v.get_z();
                // Verificamos si el punto cae dentro del círculo de la tapa
                float dist_2 = (x_impacto - centro.get_x()) * (x_impacto - centro.get_x()) +
                               (z_impacto - centro.get_z()) * (z_impacto - centro.get_z());
                if (dist_2 <= radio * radio) {
                    t_min = t_sup;
                    hubo_interseccion = true;
                }
            }
        }

        return hubo_interseccion ? t_min : -1.0f;
	}

	Vector normal_en_punto(Vector p) override {
        const float EPSILON = 0.001f;

        // 1. Tapa superior
        if (p.get_y() >= (centro.get_y() + altura) - EPSILON) {
            return Vector(0.0f, 1.0f, 0.0f);
        }
        // 2. Tapa inferior
        if (p.get_y() <= centro.get_y() + EPSILON) {
            return Vector(0.0f, -1.0f, 0.0f);
        }

        // 3. Cuerpo lateral (proyectamos la normal hacia afuera en X y Z)
        Vector de_centro_a_p = p - centro;
        Vector normal_lateral(de_centro_a_p.get_x(), 0.0f, de_centro_a_p.get_z());

        return normal_lateral.normal();
    }
};

/*
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
	Color color, ambiente;
public:
	Escena(Color color_fondo, Color luz_ambiente)
		: color(color_fondo), ambiente(luz_ambiente) {};
	Escena(string archivo);

	void agregar(Objeto *o) {
		lista_objetos.push_back(o);
	}

	void agregar(Luz *l) {
		lista_luces.push_back(l);
	}

	const vector<Objeto *> &objetos() {
		return lista_objetos;
	}

	const vector<Luz *> &luces() {
		return lista_luces;
	}

	Color color_fondo() {
		return color;
	}

	Color luz_ambiente() {
		return ambiente;
	}
};

class Rayo {
private:
	Vector punto_inicial, direccion;
	Escena *escena;
	Objeto *evitado, *adentro;

	Color sombra(Objeto *objeto, float t, int profundidad) {
		Vector punto = punto_inicial + t * direccion;
		Vector normal = objeto->normal_en_punto(punto).normal();
		//if (-normal.producto_interno(direccion) < 1e-3)
	    //		return escena->color_fondo();

		Color color = multiplicacion(objeto->luz_ambiente(), escena->luz_ambiente());

		for (const auto &luz : escena->luces()) {
			Vector direccion_a_luz = (luz->get_posicion() - punto).normal();
			float producto = normal * direccion_a_luz;

			if (producto > 0) {
				// Calcular luz difusa
				color = suma(color, multiplicacion(multiplicacion(objeto->luz_difusa(), luz->luz_difusa()), producto));

				// Calcular luz especular
				Vector h = (direccion_a_luz - direccion.normal()).normal();
				float producto_especular = powf(normal * h, 100.0f);

				color = suma(color, multiplicacion(multiplicacion(objeto->luz_especular(), luz->luz_especular()), producto_especular));
			}

			if (profundidad > 0) {
				if (objeto->get_reflejante()) {
					Rayo r(punto, direccion.reflexion(normal), escena);

					r.evitar(objeto);
					if (adentro != nullptr)
						r.adentro_de(adentro);

					Color color_reflexion = multiplicacion(objeto->luz_especular(), r.color(profundidad - 1));
					color = suma(color, multiplicacion(color_reflexion, 0.4));
				}

				if (objeto->get_transparencia() < 1) {
					float nabla1 = adentro == nullptr ? 1 : adentro->get_refraccion(),
						  nabla2 = objeto == adentro ? 1 : objeto->get_refraccion();
				}
			}
		}

		return color;
	}
public:
	Rayo(Vector punto_inicial, Vector direccion, Escena *escena)
		: punto_inicial(punto_inicial), direccion(0.01*direccion.normal()),
		  escena(escena), evitado(nullptr), adentro(nullptr) {}

	// Especifica un objeto a evitar al calcular intersecciones.
	void evitar(Objeto *o) {
		evitado = o;
	}

	// Especifica adentro de qué objeto se está. Si se está adentro de un objeto o,
	// no se va a evitar aunque se llame a evitar(o).
	void adentro_de(Objeto *o) {
		adentro = o;
	}

	Color color(int profundidad) {
		int distancia_minima = numeric_limits<float>::infinity();
		Objeto *objeto_mas_cercano = nullptr;

		for (auto objeto : escena->objetos()) {
			if (objeto == evitado && objeto != adentro)
				continue;

			float distancia = objeto->interseccion_mas_cercana(punto_inicial, direccion);

			if (distancia < distancia_minima && distancia > 0) {
				objeto_mas_cercano = objeto;
				distancia_minima = distancia;
			}
		}

		if (objeto_mas_cercano != nullptr) {
			return sombra(objeto_mas_cercano, distancia_minima, profundidad);
		} else {
			return escena->color_fondo();
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
		pixeles.clear();
		pixeles.reserve(largo * alto);

		for (int i = 0; i < alto; i++) {
			for (int j = 0; j < largo; j++) {
				Vector direccion((i - alto / 2.0) / alto , (j - largo / 2.0) / alto, 1);

				Rayo r(posicion_camara, direccion, escena);
				pixeles.push_back(r.color(4));
			}
		}

		return pixeles;
	}

	// Guarda la imagen en un archivo
	void guardar(string archivo);
};

int main() {
	Color color_ambiente({0, 0, 0});
	Color luz_ambiente({50, 50, 50});
	Escena escena(color_ambiente, luz_ambiente);

	Vector posicion_esfera(0, -1, 6);
	Color color({255, 0, 0});
	Esfera e(posicion_esfera, 1, false, 1, 1, color, color, {100, 100, 100});
	escena.agregar(&e);

	Vector posicion_esfera2(-1, 1, 7);
	Color gris({150, 150, 150});
	Esfera e2(posicion_esfera2, 0.5, true, 1, 1, gris, gris, {255, 255, 255});
	escena.agregar(&e2);

	// --- AGREGANDO PAREDES, TECHO Y PISO ---
    Color color_gris({150, 150, 150});
    Color color_pared_izq({0, 255, 0});  
    Color color_pared_der({0, 0, 255});  

    // 1. Piso (Ubicado abajo en Y = -3, normal mira hacia arriba [0, 1, 0])
    Plano piso(Vector(0, -3, 0), Vector(0, 1, 0), false, 1, color_gris, color_gris, {50, 50, 50});
    escena.agregar(&piso);

    // 2. Techo (Ubicado arriba en Y = 3, normal mira hacia abajo [0, -1, 0])
    Plano techo(Vector(0, 3, 0), Vector(0, -1, 0), false, 1, color_gris, color_gris, {50, 50, 50});
    escena.agregar(&techo);

    // 3. Pared Izquierda (Ubicada en X = -4, normal mira hacia la derecha [1, 0, 0])
    Plano pared_izq(Vector(-4, 0, 0), Vector(1, 0, 0), false, 1, color_gris, color_gris, {50, 50, 50});
    escena.agregar(&pared_izq);

    // 4. Pared Derecha (Ubicada en X = 4, normal mira hacia la izquierda [-1, 0, 0])
    Plano pared_der(Vector(4, 0, 0), Vector(-1, 0, 0), false, 1, color_gris, color_gris, {50, 50, 50});
    escena.agregar(&pared_der);

    // 5. Pared del Fondo (Ubicada atrás de la esfera en Z = 12, normal mira hacia la cámara [0, 0, -1])
    Plano fondo(Vector(0, 0, 12), Vector(0, 0, -1), false, 1, color_gris, color_gris, {50, 50, 50});
    escena.agregar(&fondo);

	Vector posicion_luz(0, 2, 2);
	Color color_luz({255, 255, 255});
	Luz l(posicion_luz, {200, 200, 200}, {100, 100, 100});
	escena.agregar(&l);

	int largo = 2000, alto = 1000;

	Vector posicion_camara(0, 0, 0);
	Imagen imagen(&escena, largo, alto, posicion_camara);

	guardar_render_a_png(imagen.dibujar(), largo, alto, "foto.png");
}
