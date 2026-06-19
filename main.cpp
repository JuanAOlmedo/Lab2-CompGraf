#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <limits>
#include <FreeImage.h>
#include <nlohmann/json.hpp>
#include <fstream>

using namespace std;
using namespace nlohmann::json_abi_v3_12_0;

class Vector {
public:
	float x, y, z;

	Vector()
		: x(0), y(0), z(0) {}

	Vector(float x, float y, float z)
		: x(x), y(y), z(z) {}

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
	        Vector ref = (std::abs(n.x) < 0.9) ? Vector(1,0,0) : Vector(0,1,0);
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

void from_json(const json& j, Vector& vector) {
    if (!j.is_array() || j.size() != 3) {
        throw json::type_error::create(
            302,
            "Los vectores son arreglos de tres elementos",
            &j
        );
    }

    vector.x = j.at(0).get<float>();
    vector.y = j.at(1).get<float>();
    vector.z = j.at(2).get<float>();
}

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

void from_json(const json& j, Color& color) {
    if (!j.is_array() || j.size() != 3) {
        throw json::type_error::create(
            302,
            "Los colores son arreglos de tres elementos",
            &j
        );
    }

    color.r = j.at(0).get<float>();
    color.g = j.at(1).get<float>();
    color.b = j.at(2).get<float>();
}

class Luz {
private:
	Vector posicion;
	Color difusa, especular;
public:
	Luz(Vector posicion, Color difusa, Color especular)
		: posicion(posicion), difusa(difusa), especular(especular) {}

	Luz(const json& j) {
		posicion = j["posicion"].get<Vector>();
		difusa = j["difusa"].get<Color>();
		especular = j["especular"].get<Color>();
	}

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

	Objeto(const json& j) {
		reflejante = j["espejado"].get<bool>();
		transparencia = j["transparencia"].get<float>();
		refraccion = j["refraccion"].get<float>();
		ambiente = j["luz"]["ambiente"].get<Color>();
		difusa = j["luz"]["difusa"].get<Color>();
		especular = j["luz"]["especular"].get<Color>();
	}

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

	Esfera(const json& j) : Objeto(j) {
		centro = j["posicion"].get<Vector>();
		radio = j["radio"].get<float>();
	}

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
	
	Plano(const json& j) : Objeto(j) {
		punto_plano = j["punto"].get<Vector>();
		normal_plano = j["normal"].get<Vector>();
	}

	float interseccion_mas_cercana(const Vector &p, const Vector &v) const override {
		float denominador = v * normal_plano;

		// Si el denominador es casi cero, el rayo es paralelo al plano
        if (std::abs(denominador) < 1e-6f) {
            return -1.0f;
        }

		// fórmula: t = ((q - p) . n) / (v . n)
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
	
	Cilindro(const json& j) : Objeto(j) {
		centro = j["posicion"].get<Vector>();
		radio = j["radio"].get<float>();
		altura = j["altura"].get<float>();
	}

	float interseccion_mas_cercana(const Vector &p, const Vector &v) const override {
		float t_min = std::numeric_limits<float>::infinity();
		bool hubo_interseccion = false;

		// --- 1. INTERSECCIÓN CON EL CUERPO LATERAL ---
        float a = v.x * v.x + v.z * v.z;
        float b = 2.0f * ((p.x - centro.x) * v.x + (p.z - centro.z) * v.z);
        float c = (p.x - centro.x) * (p.x - centro.x) + 
                  (p.z - centro.z) * (p.z - centro.z) - radio * radio;

        float det = b * b - 4.0f * a * c;

        if (det >= 0.0f) {
            float t1 = (-b - std::sqrt(det)) / (2.0f * a);
            float t2 = (-b + std::sqrt(det)) / (2.0f * a);

            // Revisamos ambas soluciones de la cuadrática
            for (float t : {t1, t2}) {
                if (t > 1e-4f && t < t_min) {
                    // Calculamos la altura Y del punto de impacto
                    float y_impacto = p.y + t * v.y;
                    // Validamos si cae dentro de la altura del cilindro
                    if (y_impacto >= centro.y && y_impacto <= centro.y + altura) {
                        t_min = t;
                        hubo_interseccion = true;
                    }
                }
            }
        }

        // --- 2. INTERSECCIÓN CON LAS TAPAS (PLANOS LIMITADOS) ---
        // Tapa Inferior (Plano en Y = centro.y)
        if (std::abs(v.y) > 1e-6f) {
            float t_inf = (centro.y - p.y) / v.y;
            if (t_inf > 1e-4f && t_inf < t_min) {
                float x_impacto = p.x + t_inf * v.x;
                float z_impacto = p.z + t_inf * v.z;
                // Verificamos si el punto cae dentro del círculo de la tapa
                float dist_2 = (x_impacto - centro.x) * (x_impacto - centro.x) +
                               (z_impacto - centro.z) * (z_impacto - centro.z);
                if (dist_2 <= radio * radio) {
                    t_min = t_inf;
                    hubo_interseccion = true;
                }
            }

            // Tapa Superior (Plano en Y = centro.y + altura)
            float t_sup = ((centro.y + altura) - p.y) / v.y;
            if (t_sup > 1e-4f && t_sup < t_min) {
                float x_impacto = p.x + t_sup * v.x;
                float z_impacto = p.z + t_sup * v.z;
                // Verificamos si el punto cae dentro del círculo de la tapa
                float dist_2 = (x_impacto - centro.x) * (x_impacto - centro.x) +
                               (z_impacto - centro.z) * (z_impacto - centro.z);
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
        if (p.y >= (centro.y + altura) - EPSILON) {
            return Vector(0.0f, 1.0f, 0.0f);
        }
        // 2. Tapa inferior
        if (p.y <= centro.y + EPSILON) {
            return Vector(0.0f, -1.0f, 0.0f);
        }

        // 3. Cuerpo lateral (proyectamos la normal hacia afuera en X y Z)
        Vector de_centro_a_p = p - centro;
        Vector normal_lateral(de_centro_a_p.x, 0.0f, de_centro_a_p.z);

        return normal_lateral.normal();
    }
};

struct CuadrilateroData {
    int v0, v1, v2, v3; // Los 4 índices de los vértices en sentido horario o antihorario
};

void from_json(const json& j, CuadrilateroData& c) {
    if (!j.is_array() || j.size() != 4) {
        throw json::type_error::create(
            302,
            "Los cuadriláteros son arreglos de cuatro elementos",
            &j
        );
    }

    c.v0 = j.at(0).get<float>();
    c.v1 = j.at(1).get<float>();
    c.v2 = j.at(2).get<float>();
    c.v3 = j.at(3).get<float>();
}

class MallaCuadrilateros : public Objeto {
private:
    vector<Vector> vertices;
    vector<CuadrilateroData> caras;
    mutable Vector normal_ultimo_impacto; // mutable permite modificarla dentro de métodos const

    // Función auxiliar interna corregida con paso por referencia constante
    static float interseccion_triangulo(const Vector& p, const Vector& v, const Vector& v0, const Vector& v1, const Vector& v2, Vector& normal_out) {
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

	MallaCuadrilateros(const json& j) : Objeto(j) {
		vertices = j["vertices"].get<vector<Vector>>();
		caras = j["caras"].get<vector<CuadrilateroData>>();
	}

    float interseccion_mas_cercana(const Vector &p, const Vector &v) const override {
        float t_min = std::numeric_limits<float>::infinity();
        bool hubo_impacto = false;
        Vector normal_temporal(0, 1, 0);

        for (const auto& cara : caras) {
            Vector p0 = vertices[cara.v0];
            Vector p1 = vertices[cara.v1];
            Vector p2 = vertices[cara.v2];
            Vector p3 = vertices[cara.v3];

            float tA = interseccion_triangulo(p, v, p0, p1, p2, normal_temporal);
            if (tA > 1e-4f && tA < t_min) {
                t_min = tA;
                normal_ultimo_impacto = normal_temporal;
                hubo_impacto = true;
            }

            float tB = interseccion_triangulo(p, v, p0, p2, p3, normal_temporal);
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
		
	Escena(json j) {
	    color = j["color_fondo"].get<Color>();
	    ambiente = j["luz_ambiente"].get<Color>();

	    for (const auto& esfera : j["objetos"]["esferas"]) {
	    	Objeto *o = new Esfera(esfera);
	    	lista_objetos.push_back(o);
	    }

	    for (const auto& cilindro : j["objetos"]["cilindros"]) {
	    	Objeto *o = new Cilindro(cilindro);
	    	lista_objetos.push_back(o);
	    }

	    for (const auto& plano : j["objetos"]["planos"]) {
	    	Objeto *o = new Plano(plano);
	    	lista_objetos.push_back(o);
	    }

	    for (const auto& malla : j["objetos"]["mallas"]) {
	    	Objeto *o = new MallaCuadrilateros(malla);
	    	lista_objetos.push_back(o);
	    }

	    for (const auto& luz : j["luces"]) {
	    	Luz *l = new Luz(luz);
	    	lista_luces.push_back(l);
	    }
	}

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

					color += objeto->luz_especular() * r.color(profundidad - 1) * 0.6;
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

	

public:
	Rayo(const Vector punto_inicial, const Vector direccion, const Escena *escena)
		: punto_inicial(punto_inicial), direccion(direccion),
		  escena(escena), evitado(nullptr), adentro(nullptr) {}

	// Devuelve el objeto más cercano al rayo y la distancia a él.
	// Si no interseca ningún objeto, devuelve {nullptr, infinity}.
	pair<const Objeto *, float> objeto_mas_cercano() const {
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
	Vector posicion_camara, direccion_vista, up;

	vector<Color> pixeles;
public:
	Imagen(const Escena *escena, int largo, int alto,
		   const Vector posicion_camara, const Vector direccion_vista, const Vector up)
		: escena(escena), largo(largo), alto(alto), posicion_camara(posicion_camara),
		  direccion_vista(direccion_vista.normal()), up(up.normal()) {}

	Imagen(const Escena *escena, const json &j) : escena(escena) {
		largo = j["largo_imagen"].get<int>();
		alto = j["alto_imagen"].get<int>();
		posicion_camara = j["posicion_camara"].get<Vector>();
		direccion_vista = j["direccion_vista"].get<Vector>();
		up = j["direccion_arriba"].get<Vector>().normal();
	}

	// Dibuja la escena y devuelve los pixeles (el tamaño del vector es largo * alto)
	vector<Color> dibujar() {
		pixeles.clear();
		pixeles.reserve(largo * alto);
		Vector direccion_barrido = up.producto_vectorial(direccion_vista.normal());

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

	void generar_mapa_reflexion(string archivo) {
		pixeles.clear();
		pixeles.reserve(largo * alto);
		Vector direccion_barrido = up.producto_vectorial(direccion_vista);

		for (int i = 0; i < alto; i++) {
			for (int j = 0; j < largo; j++) {
				// Generamos el rayo primario de la cámara exactamente igual que en dibujar()
				Vector direccion =
					direccion_vista
					+ direccion_barrido * (j - largo / 2.0) / alto
					+ up * (alto / 2.0 - i) / alto;

				Rayo r(posicion_camara, direccion.normal(), escena);
				
				// Buscamos cuál es el objeto más cercano con el que choca el rayo
				auto mas_cercano = r.objeto_mas_cercano();

				Color color_gris({0.0f, 0.0f, 0.0f}); // Negro por defecto (fondo)

				if (mas_cercano.first != nullptr) {
					// Si hay impacto, evaluamos si el objeto es reflejante
					// true -> Blanco (255), false -> Negro (0)
					if (mas_cercano.first->get_reflejante()) {
						color_gris = {255.0f, 255.0f, 255.0f}; // Blanco total
					} else {
						color_gris = {0.0f, 0.0f, 0.0f};       // Negro total
					}
				}

				pixeles.push_back(color_gris);
			}
		}

		// Reutilizamos la lógica exacta de FreeImage de tu método guardar() para salvar este mapa
		FreeImage_Initialise();
		FIBITMAP *imagen = FreeImage_Allocate(largo, alto, 24);

		if (!imagen) {
			FreeImage_DeInitialise();
			cerr << "Error de FreeImage en mapa de reflexión" << endl; 
			exit(1);
		}

		for (int i = 0; i < alto; i++) {
			BYTE *fila = FreeImage_GetScanLine(imagen, i);

			for (int j = 0; j < largo; j++) {
				int indice = (alto - 1 - i) * largo + j;

				fila[j * 3 + 0] = static_cast<unsigned char>(pixeles[indice].b);
				fila[j * 3 + 1] = static_cast<unsigned char>(pixeles[indice].g);
				fila[j * 3 + 2] = static_cast<unsigned char>(pixeles[indice].r);
			}
		}

		FreeImage_Save(FIF_PNG, imagen, archivo.data(), PNG_DEFAULT);

		FreeImage_Unload(imagen);
		FreeImage_DeInitialise();
	}

	// Genera la imagen auxiliar para los coeficientes de transmisión (transparencia) en blanco y negro
	void generar_mapa_transmision(string archivo) {
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
				
				auto mas_cercano = r.objeto_mas_cercano();

				Color color_gris({0.0f, 0.0f, 0.0f}); // Negro por defecto (fondo/opaco)

				if (mas_cercano.first != nullptr) {
					// Extraemos el valor de transparencia del objeto (va de 0.0 a 1.0)
					float t = mas_cercano.first->get_transparencia();

					// AJUSTE DE ESCALA: 
					// Si en tu código 0.0 es totalmente opaco y 1.0 es totalmente transparente, dejamos 't' directo.
					// Si en tu código es al revés (0.0 transparente, 1.0 opaco), descomentá la siguiente línea:
					// t = 1.0f - t;
					t = 1.0f - t;
					// Multiplicamos por 255 para llevarlo al rango de color RGB
					float gris_val = t * 255.0f;
					color_gris = {gris_val, gris_val, gris_val};
				}

				pixeles.push_back(color_gris);
			}
		}

		// Guardamos la imagen en disco usando FreeImage
		FreeImage_Initialise();
		FIBITMAP *imagen = FreeImage_Allocate(largo, alto, 24);

		if (!imagen) {
			FreeImage_DeInitialise();
			cerr << "Error de FreeImage en mapa de transmisión" << endl; 
			exit(1);
		}

		for (int i = 0; i < alto; i++) {
			BYTE *fila = FreeImage_GetScanLine(imagen, i);

			for (int j = 0; j < largo; j++) {
				int indice = (alto - 1 - i) * largo + j;

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
    ifstream archivo("escena.json");
    json j = json::parse(archivo);

	Escena escena(j);
	Imagen imagen(&escena, j);

	imagen.dibujar();
	imagen.guardar("foto.png");

	imagen.generar_mapa_reflexion("mapa_reflexion.png");
	imagen.generar_mapa_transmision("mapa_transmision.png");
}
