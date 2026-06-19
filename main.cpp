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
	Vector()
		: x(0), y(0), z(0) {}

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

	Vector cambiar_angulo(const Vector& n, float theta) const {
		// n tiene que tener norma 1
	    // Componente de direccion perpendicular a normal, dentro del plano
	    Vector proj = (n * *this) * n;
	    Vector perp = *this - proj;

	    float norma_perp = perp.get_norma();
	    if (norma_perp < 1e-9) {
	        // direccion es paralelo (o antiparalelo) a normal: el plano no está
	        // determinado, elegimos un perpendicular arbitrario
	        Vector ref = (std::abs(n.get_x()) < 0.9) ? Vector(1,0,0) : Vector(0,1,0);
	        perp = n.producto_vectorial(ref);
	    	norma_perp = perp.get_norma();
	    }
	    perp = perp / norma_perp;

	    // Nueva direccion unitaria a ángulo theta de normal, en el mismo plano
	    return n * std::cos(theta) + perp * std::sin(theta);
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
    float r, g, b;

    Color operator*(const Color& other) const {
        return {
            std::min(r * other.r / 255.0f, 255.0f),
            std::min(g * other.g / 255.0f, 255.0f),
            std::min(b * other.b / 255.0f, 255.0f)
        };
    }

    Color operator*(float x) const {
        return {
            std::min(r * x, 255.0f),
            std::min(g * x, 255.0f),
            std::min(b * x, 255.0f)
        };
    }

    Color operator+(const Color& other) const {
        return {
            std::min(r + other.r, 255.0f),
            std::min(g + other.g, 255.0f),
            std::min(b + other.b, 255.0f)
        };
    }

    void operator*=(float x) {
        r = std::min(r * x, 255.0f);
        g = std::min(g * x, 255.0f);
        b = std::min(b * x, 255.0f);
    }

    void operator*=(const Color& other) {
        r = std::min(r * other.r / 255.0f, 255.0f);
        g = std::min(g * other.g / 255.0f, 255.0f);
        b = std::min(b * other.b / 255.0f, 255.0f);
    }

    void operator+=(const Color& other) {
        r = std::min(r + other.r, 255.0f);
        g = std::min(g + other.g, 255.0f);
        b = std::min(b + other.b, 255.0f);
    }

    void operator-=(const Color& other) {
        r = std::max(r - other.r, 0.0f);
        g = std::max(g - other.g, 0.0f);
        b = std::max(b - other.b, 0.0f);
    }
};

class Luz {
private:
	Vector posicion;
	Color difusa, especular;
public:
	Luz(Vector posicion, Color difusa, Color especular)
		: posicion(posicion), difusa(difusa), especular(especular) {}

	Vector get_posicion() const {
		return posicion;
	}

	Color luz_difusa() const {
		return difusa;
	}

	Color luz_especular() const {
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
	// Devuelve negativo si no existe.
	virtual float interseccion_mas_cercana(const Vector &p, const Vector &v) const = 0;
	// Devuelve la normal del objeto en el punto p.
	virtual Vector normal_en_punto(const Vector &p) const = 0;
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
	float interseccion_mas_cercana(const Vector &p, const Vector &v) const override {
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

		det = sqrt(det);

		float t1 = (-b - det) / (2 * a);

		if (t1 > 1e-6)
			return t1;

		float t2 = (-b + det) / (2 * a);

		if (t2 > 1e-6)
			return t2;

		return -1;
	}

	Vector normal_en_punto(const Vector &p) const override {
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
	
	float interseccion_mas_cercana(const Vector &p, const Vector &v) const override {
		float denominador = v * normal_plano;

		// Si el denominador es casi cero, el rayo es paralelo al plano
        if (std::abs(denominador) < 1e-6f) {
            return -1.0f;
        }

		//fórmula: t = ((q - p) . n) / (v . n)
        float t = ((punto_plano - p) * normal_plano) / denominador;

		// Si t es positivo, la intersección ocurrió adelante de la cámara
        if (t > 1e-6f)
            return t;

        return -1.0f;
	}

	Vector normal_en_punto(const Vector &p) const override {
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
	Cilindro(Vector c, float r, float h, bool reflectante, float transparencia,float refraccion,
			Color ambiente, Color difusa, Color especular)
			: centro(c), radio(r), altura(h), Objeto(reflectante, transparencia, refraccion ,ambiente,difusa, especular){}
	
	float interseccion_mas_cercana(const Vector &p, const Vector &v) const override {
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

	

	Vector normal_en_punto(const Vector &p) const override {
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

struct CuadrilateroData {
    int v0, v1, v2, v3; // Los 4 índices de los vértices en sentido horario o antihorario
};

class MallaCuadrilateros : public Objeto {
private:
    std::vector<Vector> vertices;
    std::vector<CuadrilateroData> caras;
    mutable Vector normal_ultimo_impacto; // mutable permite modificarla dentro de métodos const

    // Función auxiliar interna corregida con paso por referencia constante
    float intersección_triangulo(const Vector& p, const Vector& v, const Vector& v0, const Vector& v1, const Vector& v2, Vector& normal_out) const {
        Vector edge1 = v1 - v0;
        Vector edge2 = v2 - v0;
        Vector h = v.producto_vectorial(edge2);
        float a = edge1.producto_interno(h);

        if (a > -1e-6f && a < 1e-6f) return -1.0f;

        float f = 1.0f / a;
        Vector s = p - v0;
        float u = f * s.producto_interno(h);
        if (u < 0.0f || u > 1.0f) return -1.0f;

        Vector q = s.producto_vectorial(edge1);
        float _v = f * v.producto_interno(q);
        if (_v < 0.0f || u + _v > 1.0f) return -1.0f;

        float t = f * edge2.producto_interno(q);
        if (t > 1e-4f) {
            normal_out = edge1.producto_vectorial(edge2).normal();
            return t;
        }
        return -1.0f;
    }

public:
    MallaCuadrilateros(const std::vector<Vector>& vertices, const std::vector<CuadrilateroData>& caras,
                       bool reflejante, float transparencia, float refraccion,
                       Color ambiente, Color difusa, Color especular)
        : vertices(vertices), caras(caras), normal_ultimo_impacto(0,1,0),
          Objeto(reflejante, transparencia, refraccion, ambiente, difusa, especular) {}

    // Ahora SÍ coincide exactamente con la firma de Objeto
    float interseccion_mas_cercana(const Vector &p, const Vector &v) const override {
        float t_min = std::numeric_limits<float>::infinity();
        bool hubo_impacto = false;
        Vector normal_temporal(0, 1, 0);

        for (const auto& cara : caras) {
            Vector p0 = vertices[cara.v0];
            Vector p1 = vertices[cara.v1];
            Vector p2 = vertices[cara.v2];
            Vector p3 = vertices[cara.v3];

            float tA = intersección_triangulo(p, v, p0, p1, p2, normal_temporal);
            if (tA > 1e-4f && tA < t_min) {
                t_min = tA;
                normal_ultimo_impacto = normal_temporal;
                hubo_impacto = true;
            }

            float tB = intersección_triangulo(p, v, p0, p2, p3, normal_temporal);
            if (tB > 1e-4f && tB < t_min) {
                t_min = tB;
                normal_ultimo_impacto = normal_temporal;
                hubo_impacto = true;
            }
        }

        return hubo_impacto ? t_min : -1.0f;
    }

    // Firma corregida para que coincida con Objeto
    Vector normal_en_punto(const Vector &p) const override {
        return normal_ultimo_impacto;
    }
};

class Escena {
private:
	vector<const Objeto *> lista_objetos;
	vector<const Luz *> lista_luces;
	Color color, ambiente;
public:
	Escena(Color color_fondo, Color luz_ambiente)
		: color(color_fondo), ambiente(luz_ambiente) {};
	Escena(string archivo);

	void agregar(const Objeto *o) {
		lista_objetos.push_back(o);
	}

	void agregar(const Luz *l) {
		lista_luces.push_back(l);
	}

	const vector<const Objeto *> &objetos() const {
		return lista_objetos;
	}

	const vector<const Luz *> &luces() const {
		return lista_luces;
	}

	Color color_fondo() const {
		return color;
	}

	Color luz_ambiente() const {
		return ambiente;
	}
};

class Rayo {
private:
	const float INDICE_REFRACCION_AIRE = 1;
	const float COEFICIENTE_ESPECULAR = 100;

	const Vector punto_inicial, direccion;
	const Escena *escena;
	const Objeto *evitado; // No queremos calcular intersecciones con este objeto
	const Objeto *adentro; // Estamos adentro de este objeto

	Color oclusion(float distancia_maxima) const {
		Color color({255, 255, 255});

		for (const auto objeto : escena->objetos()) {
			if (objeto == evitado)
				continue;

			float distancia = objeto->interseccion_mas_cercana(punto_inicial, direccion);

			if (0 < distancia && distancia < distancia_maxima) {
				if (objeto->get_transparencia() == 1)
					return {0, 0, 0};

				color *= objeto->luz_difusa() * (1 / (5 * objeto->get_transparencia()));
			}
		}

		return color;
	}

	Color sombra(const Objeto *objeto, float d, int profundidad) const {
		Vector punto = punto_inicial + d * direccion;
		Vector normal = objeto->normal_en_punto(punto);

		Color color = objeto->luz_ambiente() * escena->luz_ambiente();

		for (const auto luz : escena->luces()) {
			Vector direccion_a_luz = luz->get_posicion() - punto;
			float distancia_luz = direccion_a_luz.get_norma();
			direccion_a_luz = direccion_a_luz / distancia_luz;

			float producto = normal * direccion_a_luz;
			Rayo l(punto, direccion_a_luz, escena);
			l.evitar(objeto);
			Color oclusion_luz = l.oclusion(distancia_luz);

			if (producto > 0) {
				// Calcular luz difusa
				color += oclusion_luz * objeto->luz_difusa() * luz->luz_difusa() * producto;

				// Calcular luz especular
				Vector h = (direccion_a_luz - direccion).normal();
				float producto_especular = powf(normal * h, COEFICIENTE_ESPECULAR);

				color += oclusion_luz * objeto->luz_especular() * luz->luz_especular() * producto_especular;
			}

			if (profundidad > 0) {
				if (objeto->get_reflejante()) {
					Rayo r(punto, direccion.reflexion(normal), escena);

					// Evitar que r interseque el mismo objeto
					if (adentro != objeto)
						r.evitar(objeto);
					// r va a estar sí o sí adentro del mismo objeto que yo
					if (adentro != nullptr)
						r.adentro_de(adentro);

					color += objeto->luz_especular() * r.color(profundidad - 1) * 0.4;
				}

				if (objeto->get_transparencia() < 1) {
					Vector normal_efectiva;
					float nabla1, nabla2;

					// Si no estamos adentro de ningún objeto, el primer índice de refracción
					// es el del aire. Si no, es el del objeto en el que estamos.
					if (adentro == nullptr)
						nabla1 = INDICE_REFRACCION_AIRE;
					else
						nabla1 = adentro->get_refraccion();

					if (objeto == adentro) {
						// Si estamos adentro del objeto que intersecamos, el segundo índice de
						// refracción es el aire.
						nabla2 = INDICE_REFRACCION_AIRE;
						normal_efectiva = normal;
					} else {
						// Si no, el segundo índice de refracción es el del objeto intersecado.
						nabla2 = objeto->get_refraccion();
						// La normal está apuntando en dirección opuesta a la dirección de entrada,
						// invertirla
						normal_efectiva = -normal;
					}

					float theta1 = normal_efectiva.angulo(direccion);

					// No calcular nada si hay refracción interna total
					if (nabla1 <= nabla2 || theta1 <= std::asin(nabla2 / nabla1)) {
						float theta2 = std::asin(std::sin(theta1) * nabla1 / nabla2);

						Rayo t(punto + direccion * 0.0001, direccion.cambiar_angulo(normal_efectiva, theta2), escena);

						if (objeto == adentro)
							t.evitar(objeto); // Estamos saliendo del objeto, hacer que t lo evite
						else
							t.adentro_de(objeto); // Estamos entrando al objeto

						color += objeto->luz_difusa() * t.color(profundidad - 1);
					}
				}
			}
		}
		return color;
	}

	// Devuelve el objeto más cercano al rayo y la distancia a él.
	// Si no interseca ningún objeto, devuelve {nullptr, infinity}.
	std::pair<const Objeto *, float> objeto_mas_cercano() const {
		float distancia_minima = numeric_limits<float>::infinity();
		const Objeto *mas_cercano = nullptr;

		for (auto objeto : escena->objetos()) {
			if (objeto == evitado && objeto != adentro)
				continue;

			float distancia = objeto->interseccion_mas_cercana(punto_inicial, direccion);

			if (distancia < distancia_minima && distancia > 0) {
				mas_cercano = objeto;
				distancia_minima = distancia;
			}
		}

		return {mas_cercano, distancia_minima};
	}

public:
	Rayo(const Vector punto_inicial, const Vector direccion, const Escena *escena)
		: punto_inicial(punto_inicial), direccion(direccion),
		  escena(escena), evitado(nullptr), adentro(nullptr) {}

	// Especifica un objeto a evitar al calcular intersecciones.
	void evitar(const Objeto *o) {
		evitado = o;
	}

	// Especifica adentro de qué objeto se está.
	void adentro_de(const Objeto *o) {
		adentro = o;
	}

	Color color(int profundidad) const {
		auto mas_cercano = objeto_mas_cercano();

		if (mas_cercano.first != nullptr)
			return sombra(mas_cercano.first, mas_cercano.second, profundidad);
		else
			return escena->color_fondo();
	}
};

class Imagen {
private:
	const Escena *escena;
	int largo, alto;
	const Vector posicion_camara, direccion_vista, up;

	vector<Color> pixeles;
public:
	Imagen(const Escena *escena, int largo, int alto,
		   const Vector posicion_camara, const Vector direccion_vista, const Vector up)
		: escena(escena), largo(largo), alto(alto), posicion_camara(posicion_camara),
		  direccion_vista(direccion_vista.normal()), up(up.normal()) {}

	// Dibuja la escena y devuelve los pixeles (el tamaño del vector es largo * alto)
	vector<Color> dibujar() {
		pixeles.clear();
		pixeles.reserve(largo * alto);
		Vector direccion_barrido = up.producto_vectorial(direccion_vista);

		for (int i = 0; i < alto; i++) {
			for (int j = 0; j < largo; j++) {
				Vector direccion =
					direccion_vista
					+ direccion_barrido * (j - largo / 2.0) / alto
					+ up * (alto / 2.0 - i) / alto;

				Rayo r(posicion_camara, direccion.normal(), escena);
				pixeles.push_back(r.color(3));
			}
		}

		return pixeles;
	}

	// Guarda la imagen en un archivo
	void guardar(string archivo) {
		// Inicializar librería
		FreeImage_Initialise();
		FIBITMAP *imagen = FreeImage_Allocate(largo, alto, 24);

		if (!imagen) {
			FreeImage_DeInitialise();
			cerr << "Error de FreeImage" << endl; 
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
};

int main() {
    Color color_ambiente({0, 0, 0});
    Color luz_ambiente({60, 60, 60});
    Escena escena(color_ambiente, luz_ambiente);

    // El piso está en Y = -4. El tablero de la mesa va a estar en Y = -1.0.
    // Los objetos se apoyan sobre el tablero (Y = -1.0).

    // 1. Esfera Gris Metalizada (Der)
    Vector posicion_esfera2(1.5f, -0.5f, 10.0f); // Base: -0.5 - 0.5 = -1.0
    Color gris({150, 150, 150});
    Esfera e2(posicion_esfera2, 0.5f, true, 1.0f, 1.0f, gris, gris, {255, 255, 255});
    escena.agregar(&e2);

    // 2. Esfera Rosada Transparente (Izq)
    Vector posicion_esfera3(-0.5f, -0.1f, 9.0f); // Base: -0.1 - 0.9 = -1.0
    Color rosado({150, 100, 100});
    Esfera e3(posicion_esfera3, 0.9f, false, 0.1f, 2.0f, rosado, rosado, {255, 255, 255});
    escena.agregar(&e3);

    // 3. Cilindro Verde (Centro)
    Vector posicion_cilindro(0.0f, -1.0f, 9.0f); // Arranca en Y = -1.0 y sube 1 unidad de altura
    Color color_verde({0, 255, 0});
    Cilindro cilindro(posicion_cilindro, 0.5f, 1.0f, false, 1.0f, 1.0f, color_verde, color_verde, {100, 100, 100});
    escena.agregar(&cilindro);

    // --- CONSTRUCCIÓN DE LA MESA EN EL ESPACIO REAL ---
    std::vector<Vector> v_mesa = {
        // Tablero Superior (Y = -1.0) al Inferior (Y = -1.2)
        Vector(-2.5f, -1.0f,  7.5f),  // 0
        Vector( 2.5f, -1.0f,  7.5f),  // 1
        Vector( 2.5f, -1.0f, 11.5f),  // 2
        Vector(-2.5f, -1.0f, 11.5f),  // 3
        Vector(-2.5f, -1.2f,  7.5f),  // 4
        Vector( 2.5f, -1.2f,  7.5f),  // 5
        Vector( 2.5f, -1.2f, 11.5f),  // 6
        Vector(-2.5f, -1.2f, 11.5f),  // 7

        // Pata Delantera Izquierda (De Y = -4.0 a Y = -1.2)
        Vector(-2.3f, -4.0f,  7.7f),  // 8
        Vector(-2.0f, -4.0f,  7.7f),  // 9
        Vector(-2.0f, -4.0f,  8.0f),  // 10
        Vector(-2.3f, -4.0f,  8.0f),  // 11
        Vector(-2.3f, -1.2f,  7.7f),  // 12
        Vector(-2.0f, -1.2f,  7.7f),  // 13
        Vector(-2.0f, -1.2f,  8.0f),  // 14
        Vector(-2.3f, -1.2f,  8.0f),  // 15

        // Pata Delantera Derecha (De Y = -4.0 a Y = -1.2)
        Vector( 2.0f, -4.0f,  7.7f),  // 16
        Vector( 2.3f, -4.0f,  7.7f),  // 17
        Vector( 2.3f, -4.0f,  8.0f),  // 18
        Vector( 2.0f, -4.0f,  8.0f),  // 19
        Vector( 2.0f, -1.2f,  7.7f),  // 20
        Vector( 2.3f, -1.2f,  7.7f),  // 21
        Vector( 2.3f, -1.2f,  8.0f),  // 22  <-- Arreglado el -8.0f que estaba acá!
        Vector( 2.0f, -1.2f,  8.0f)   // 23
    };

    std::vector<CuadrilateroData> c_mesa = {
        {0, 1, 2, 3}, {4, 5, 6, 7}, {0, 1, 5, 4}, {2, 3, 7, 6}, {0, 3, 7, 4}, {1, 2, 6, 5}, // Tablero
        {8,  9,  13, 12}, {9,  10, 14, 13}, {10, 11, 15, 14}, {11, 8,  12, 15},               // Pata Izq
        {16, 17, 21, 20}, {17, 18, 22, 21}, {18, 19, 23, 22}, {19, 16, 20, 23}                // Pata Der
    };

    Color color_madera({255, 255, 255});
    MallaCuadrilateros mesa(v_mesa, c_mesa, false, 1.0f, 1.0f, color_madera, color_madera, {50, 50, 50});
    escena.agregar(&mesa);

    // --- ENTORNO ---
    Color color_gris({150, 150, 150});
    Plano piso(Vector(0, -4, 0), Vector(0, 1, 0), false, 1, color_gris, color_gris, {50, 50, 50});
    escena.agregar(&piso);

    Plano techo(Vector(0, 4, 0), Vector(0, -1, 0), false, 1, color_gris, color_gris, {50, 50, 50});
    escena.agregar(&techo);

    Plano pared_izq(Vector(-4, 0, 0), Vector(1, 0, 0), false, 1, color_gris, color_gris, {50, 50, 50});
    escena.agregar(&pared_izq);

    Plano pared_der(Vector(4, 0, 0), Vector(-1, 0, 0), false, 1, color_gris, color_gris, {50, 50, 50});
    escena.agregar(&pared_der);

    Plano fondo(Vector(0, 0, 14), Vector(0, 0, -1), false, 1, color_gris, color_gris, {50, 50, 50});
    escena.agregar(&fondo);

    // Cambié la posición de las luces a Y = 3.0 para que iluminen desde el techo real
    Vector posicion_luz(-1.5f, 3.5f, 8.5f); 
    Luz l(posicion_luz, {230, 230, 230}, {80, 80, 80});
    escena.agregar(&l);

    Vector posicion_luz2(1.5f, 3.5f, 8.5f);
    Luz l2(posicion_luz2, {230, 230, 230}, {80, 80, 80});
    escena.agregar(&l2);
    int largo = 1000, alto = 1000;

	Vector posicion_camara(0, 0, 0);
	Vector direccion_vista(0, 0, 1);

	//Vector direccion_vista(0, -0.2f, 1); agacha la cabeza
	
	/*Vector posicion_camara(-3.0f, 0.5f, 7.0f); 
	Vector direccion_vista(1.0f, -0.4f, 1.0f);
	Vector up(0, 1, 0);*/

	/*Vector posicion_camara(-2.0f, 2.0f, -2.0f); // Esquina superior trasera izquierda

	
	Vector direccion_vista(0.5f, -0.5f, 9.5f); // Centro geométrico del bodegón

	Vector up(0.0f, 1.0f, 0.0f);*/
	
	Vector up(0, 1, 0);
	Imagen imagen(&escena, largo, alto, posicion_camara, direccion_vista, up);

	imagen.dibujar();
	imagen.guardar("foto.png");
}
