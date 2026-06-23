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
using json = nlohmann::json;

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

	float get_norma_inf() const {
		return max(max(x, y), z);
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

    if (color.r < 0 || 255 < color.r || color.g < 0 || 255 < color.g
    	|| color.b < 0 || 255 < color.b) {
    	throw json::type_error::create(
            302,
            "Los colores tienen que estar en el rango 0-255",
            &j
        );
    }
}

class Luz {
private:
	Vector posicion;
	Color difusa, especular;
public:
	Luz(Vector posicion, Color difusa, Color especular)
		: posicion(posicion), difusa(difusa), especular(especular) {}

	explicit Luz(const json& j)
		: posicion(j.at("posicion").get<Vector>()),
		  difusa(j.at("difusa").get<Color>()),
		  especular(j.at("especular").get<Color>()) {}

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
	Objeto(bool reflejante, float transparencia, float refraccion,
		   Color ambiente, Color difusa, Color especular)
		: reflejante(reflejante), transparencia(transparencia), refraccion(refraccion),
		  ambiente(ambiente), difusa(difusa), especular(especular) {}
	Objeto(const json& j)
	    : reflejante(j.at("espejado").get<bool>()),
	      transparencia(j.at("transparencia").get<float>()),
	      refraccion(j.at("refraccion").get<float>()),
	      ambiente(j.at("luz").at("ambiente").get<Color>()),
	      difusa(j.at("luz").at("difusa").get<Color>()),
	      especular(j.at("luz").at("especular").get<Color>()) {}
 	
 	virtual ~Objeto() = default;

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
		: Objeto(reflejante, transparencia, refraccion, ambiente, difusa, especular),
		  centro(centro), radio(radio) {}

	explicit Esfera(const json& j)
		: Objeto(j),
		  centro(j.at("posicion").get<Vector>()),
		  radio(j.at("radio").get<float>()) {}

	// Devuelve el menor t > 0 tal que p + tv está en el objeto.
	float interseccion_mas_cercana(const Vector &p, const Vector &v) const override {
		// La ecuación cuadrática a resolver se obtiene desarrollando:
		//     (p + tv - centro) * (p + tv - centro) = radio ^ 2
		float c = p.get_norma_2() + centro.get_norma_2() - 2 * p.producto_interno(centro) - radio * radio,
			  b = 2 * p.producto_interno(v) - 2 * centro.producto_interno(v),
			  a = v.get_norma_2();

		float det = b * b - 4.0f * a * c;
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
	
	explicit Plano(const json& j)
		: Objeto(j),
		  punto_plano(j.at("punto").get<Vector>()),
		  normal_plano(j.at("normal").get<Vector>().normal()) {}

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
	
	explicit Cilindro(const json& j)
		: Objeto(j),
		  centro(j.at("posicion").get<Vector>()),
		  radio(j.at("radio").get<float>()),
		  altura(j.at("altura").get<float>()) {}

	float interseccion_mas_cercana(const Vector &p, const Vector &v) const override {
		float t_min = std::numeric_limits<float>::infinity();
		bool hubo_interseccion = false;

		// Primero calculamos la intersección con el cuerpo lateral
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
                if (t > 1e-4f && t < t_min && !hubo_interseccion) {
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

        // Ahora calculamos intersección con las tapas
        if (std::abs(v.y) > 1e-6f) {
        	// Calcular intersecciones con los planos que contienen a las tapas
            float t_inf = (centro.y - p.y) / v.y;
            float t_sup = ((centro.y + altura) - p.y) / v.y;

            // Verificar si las intersecciones están dentro de las tapas y si son
            // mínimas.
            for (float t : {t_inf, t_sup}) {
	            if (t > 1e-4f && t < t_min) {
	                float x_impacto = p.x + t * v.x;
	                float z_impacto = p.z + t * v.z;
	                // Verificamos si el punto cae dentro del círculo de la tapa
	                float dist_2 = (x_impacto - centro.x) * (x_impacto - centro.x) +
	                               (z_impacto - centro.z) * (z_impacto - centro.z);
	                if (dist_2 <= radio * radio) {
	                    t_min = t;
	                    hubo_interseccion = true;
	                }
	            }
	        }
        }

        return hubo_interseccion ? t_min : -1.0f;
	}

	Vector normal_en_punto(const Vector &p) const override {
        const float EPSILON = 1e-4;

        // Tapa superior:
        if (p.y >= (centro.y + altura) - EPSILON)
            return Vector(0.0f, 1.0f, 0.0f);
        // Tapa inferior:
        if (p.y <= centro.y + EPSILON)
            return Vector(0.0f, -1.0f, 0.0f);

        // Cuerpo lateral:
        // Le sacamos la componente en y para proyectar sobre el plano y = 0
        // (y que sea normal al cuerpo)
        Vector de_centro_a_p = p - centro;
        // Dividimos entre el radio para normalizar
        return Vector(de_centro_a_p.x / radio, 0.0f, de_centro_a_p.z / radio);
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

    static float interseccion_triangulo(const Vector& p, const Vector& v,
    		const Vector& v0, const Vector& v1, const Vector& v2, Vector& normal_out) {
		const float EPSILON = 1e-5;
        Vector edge1 = v1 - v0;
        Vector edge2 = v2 - v0;

        Vector h = v.producto_vectorial(edge2);
        float det = edge1.producto_interno(h);

        if (det > EPSILON && det < -EPSILON)
        	return -1.0f;

        float inv_det = 1.0f / det;
        Vector s = p - v0;
        float u = inv_det * s.producto_interno(h);
        if (u < 0.0f - EPSILON || u > 1.0f - EPSILON)
        	return -1.0f;

        Vector q = s.producto_vectorial(edge1);
        float _v = inv_det * v.producto_interno(q);
        if (_v < 0.0f - EPSILON || u + _v > 1.0f - EPSILON)
        	return -1.0f;

        float t = inv_det * edge2.producto_interno(q);
        if (t > EPSILON) {
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

	explicit MallaCuadrilateros(const json& j)
		: Objeto(j),
		  vertices(j.at("vertices").get<vector<Vector>>()),
		  caras(j.at("caras").get<vector<CuadrilateroData>>()) {}

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
		
	explicit Escena(json j)
		: color(j.at("color_fondo").get<Color>()),
		  ambiente(j.at("luz_ambiente").get<Color>()) {
	    for (const auto& esfera : j.at("objetos")["esferas"]) {
	    	Objeto *o = new Esfera(esfera);
	    	lista_objetos.push_back(o);
	    }

	    for (const auto& cilindro : j.at("objetos")["cilindros"]) {
	    	Objeto *o = new Cilindro(cilindro);
	    	lista_objetos.push_back(o);
	    }

	    for (const auto& plano : j.at("objetos")["planos"]) {
	    	Objeto *o = new Plano(plano);
	    	lista_objetos.push_back(o);
	    }

	    for (const auto& malla : j.at("objetos")["mallas"]) {
	    	Objeto *o = new MallaCuadrilateros(malla);
	    	lista_objetos.push_back(o);
	    }

	    for (const auto& luz : j["luces"]) {
	    	Luz *l = new Luz(luz);
	    	lista_luces.push_back(l);
	    }
	}

	~Escena() {
		for (auto o : lista_objetos)
			delete o;

		for (auto l : lista_luces)
			delete l;
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

enum class ModoRender { Completo, SoloReflexion, SoloTransparencia };

class Rayo {
private:
	const float EPSILON = 1e-3;
	const float INDICE_REFRACCION_AIRE = 1;
	const float COEFICIENTE_ESPECULAR = 100;

	const Vector punto_inicial, direccion;
	const Escena &escena;
	const Objeto *adentro; // Estamos adentro de este objeto

	int profundidad;

	// Calcula qué tan tapado está el rayo por objetos.
	// Por ejemplo, si el rayo interseca algún objeto opaco en distancia
	// menor a distancia_maxima, devuelve negro. Si interseca un objeto transparente,
	// devuelve el color del objeto.
	Color oclusion(float distancia_maxima) const {
		Color color({255, 255, 255});

		for (const auto objeto : escena.objetos()) {
			float distancia = objeto->interseccion_mas_cercana(punto_inicial, direccion);

			if (0 < distancia && distancia < distancia_maxima) {
				if (objeto->get_transparencia() == 1)
					return {0, 0, 0};

				color *= objeto->luz_difusa() * (1 / (5 * objeto->get_transparencia()));
			}
		}

		return color;
	}

	Color reflexion(const Objeto *objeto, const Vector &punto, const Vector &normal) const {
		Rayo r(punto, direccion.reflexion(normal), escena, profundidad - 1);

		// r va a estar sí o sí adentro del mismo objeto que yo
		if (adentro != nullptr)
			r.adentro_de(adentro);

		return objeto->luz_especular() * r.trazar(ModoRender::Completo) * 0.5;
	}

	// Aplica la ley de Snell a un vector v que pasa de un material con
	// coeficiente de refracción nabla1 a otro con coeficiente nabla2,
	// colisionando en un punto con normal n.
	// Si ocurre refracción interna total, no devuelve nada.
	static optional<Vector> aplicar_snell(const Vector &v, const Vector &n, 
								          float nabla1, float nabla2) {
		// Calcular el ángulo de incidencia
		float theta1 = n.angulo(v);
		float theta_critico = std::asin(nabla2 / nabla1);

		if (nabla1 > nabla2 && theta1 > theta_critico)
			return std::nullopt;

		float theta2 = std::asin(std::sin(theta1) * nabla1 / nabla2);

		return v.cambiar_angulo(n, theta2);
	}

	Color transparencia(const Objeto *objeto, const Vector &punto, const Vector &normal) const {
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

		// Aplicamos la ley de snell y verificamos que no haya refracción interna total.
		auto direccion_salida = aplicar_snell(direccion, normal_efectiva, nabla1, nabla2);
		bool refraccion_interna_total = !direccion_salida.has_value();

		// No calcular nada si hay refracción interna total
		if (!refraccion_interna_total) {
			Rayo t(punto + direccion * (EPSILON + 1e-4), *direccion_salida, escena, profundidad - 1);

			if (objeto != adentro)
				t.adentro_de(objeto); // Estamos entrando al objeto, especificarlo

			return objeto->luz_difusa() * t.trazar(ModoRender::Completo);
		}

		return {0, 0, 0};
	}

	// Devuelve el color que debería tener el rayo, sabiendo que intersecó
	// el objeto especificado a distancia d del punto inicial.
	Color sombra(const Objeto *objeto, float d) const {
		Vector punto = punto_inicial + d * direccion;
		Vector normal = objeto->normal_en_punto(punto);

		Color color = objeto->luz_ambiente() * escena.luz_ambiente();

		for (const auto luz : escena.luces()) {
			Vector direccion_a_luz = luz->get_posicion() - punto;
			float distancia_luz = direccion_a_luz.get_norma();
			direccion_a_luz = direccion_a_luz / distancia_luz;

			float producto = normal * direccion_a_luz;
			Rayo l(punto, direccion_a_luz, escena, 0);
			Color oclusion_luz = l.oclusion(distancia_luz);

			// Solo calcular iluminación si la superficie apunta en dirección
			// a la fuente de luz.
			if (producto > 0) {
				// Calcular luz difusa
				color += oclusion_luz * objeto->luz_difusa() * luz->luz_difusa() * producto;

				// Calcular luz especular
				Vector h = (direccion_a_luz - direccion).normal();
				float producto_especular = powf(normal * h, COEFICIENTE_ESPECULAR);

				color += oclusion_luz * objeto->luz_especular() * luz->luz_especular() * producto_especular;
			}

			// Calcular componentes de reflexión y transparencia si corresponde,
			if (profundidad > 0) {
				if (objeto->get_reflejante())
					color += reflexion(objeto, punto, normal);

				if (objeto->get_transparencia() < 1)
					color += transparencia(objeto, punto, normal);
			}
		}

		return color;
	}

	// Devuelve el objeto más cercano al rayo y la distancia a él.
	// Si no interseca ningún objeto, devuelve {nullptr, infinity}.
	pair<const Objeto *, float> objeto_mas_cercano() const {
		float distancia_minima = numeric_limits<float>::infinity();
		const Objeto *mas_cercano = nullptr;

		for (auto objeto : escena.objetos()) {
			float distancia = objeto->interseccion_mas_cercana(punto_inicial, direccion);

			if (distancia < distancia_minima && distancia > EPSILON) {
				mas_cercano = objeto;
				distancia_minima = distancia;
			}
		}

		if (adentro != nullptr && mas_cercano == nullptr)
			return {adentro, EPSILON};
		else
			return {mas_cercano, distancia_minima - EPSILON};
	}

public:
	Rayo(const Vector punto_inicial, const Vector direccion,
		 const Escena &escena, int profundidad)
		: punto_inicial(punto_inicial), direccion(direccion),
		  escena(escena), evitado(nullptr), adentro(nullptr),
		  profundidad(profundidad) {}

	// Especifica adentro de qué objeto se está.
	void adentro_de(const Objeto *o) {
		adentro = o;
	}

	Color trazar(ModoRender modo) const {
		auto mas_cercano = objeto_mas_cercano();

		if (mas_cercano.first != nullptr) {
			Color blanco({255, 255, 255});

			if (modo == ModoRender::SoloReflexion)
				return blanco * (float) mas_cercano.first->get_reflejante();
			else if (modo == ModoRender::SoloTransparencia)
				return blanco * (1 - mas_cercano.first->get_transparencia());
			else
				return sombra(mas_cercano.first, mas_cercano.second);
		} else
			return escena.color_fondo();
	}
};

class Imagen {
	int largo, alto;
	const vector<Color> pixeles;

public:
	Imagen(int largo, int alto, vector<Color> pixeles)
		: largo(largo), alto(alto), pixeles(move(pixeles)) {}

	// Guarda la imagen en un archivo
	void guardar(const string &archivo) {
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

class Renderer {
private:
	const Escena &escena;
	int largo, alto, profundidad, celdas_aliasing;
	Vector posicion_camara, direccion_vista, up, direccion_barrido;

	Color color_pixel(ModoRender modo, float i, float j) {
		Color color({0, 0, 0});

		// Implementación de antialiasing:
		// Se crean (celdas_aliasing * celdas_aliasing) celdas y se traza
		// un rayo por cada una de ellas.
		for (int n = 0; n < celdas_aliasing; n++) {
			for (int m = 0; m < celdas_aliasing; m++) {
				// El pixel está ubicado en el punto posicion_camara + direccion.
				// Calculamos direccion separando en tres componentes perpendiculares:
				// direccion_vista, direccion_barrido y up.
				// Dividimos las últimas dos componentes entre alto para normalizar el tamaño
				// de píxeles.
				Vector direccion =
					direccion_vista
					+ direccion_barrido * (j + (float) n / celdas_aliasing - largo / 2.0) / alto
					+ up * (alto / 2.0 - i - (float) m / celdas_aliasing) / alto;

				Rayo r(posicion_camara + direccion, direccion.normal(), escena, profundidad);
				color += r.trazar(modo) * (1.0 / celdas_aliasing / celdas_aliasing);
			}
		}

		return color;
	}
public:
	Renderer(const Escena &escena, const json &j)
		: escena(escena),
		  largo(j.at("largo_imagen").get<int>()),
		  alto(j.at("alto_imagen").get<int>()),
		  profundidad(j.at("recursion").get<int>()),
		  celdas_aliasing(j.at("celdas_para_aliasing").get<int>()),
		  posicion_camara(j.at("posicion_camara").get<Vector>()),
		  direccion_vista(j.at("direccion_vista").get<Vector>()),
		  up(j.at("direccion_arriba").get<Vector>().normal()),
		  direccion_barrido(up.producto_vectorial(direccion_vista.normal())) {
		if (largo <= 0 || alto <= 0) {
			cerr << "El largo y alto de la imagen tiene que ser positivo" << endl;
			exit(1);
		}
		if (profundidad <= 0) {
			cerr << "La profundidad de recursión tiene que ser positiva" << endl;
			exit(1);
		}
		if (celdas_aliasing <= 0) {
			cerr << "La cantidad de celdas para aliasing tiene que ser positiva" << endl;
			exit(1);
		}
	}

	// Dibuja la escena y devuelve la imagen correspondiente
	Imagen dibujar(ModoRender modo) {
		vector<Color> pixeles;
		pixeles.reserve(largo * alto);

		// Crear largo * alto píxeles y calcular el color de cada uno.
		for (int i = 0; i < alto; i++) {
			for (int j = 0; j < largo; j++) {
				pixeles.push_back(color_pixel(modo, i, j));
			}
		}

		return Imagen(largo, alto, move(pixeles));
	}
};

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
