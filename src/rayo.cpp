#include <rayo.hpp>

Color Rayo::oclusion(float distancia_maxima) const {
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

Color Rayo::reflexion(const Objeto *objeto, const Vector &punto, const Vector &normal) const {
	Rayo r(punto, direccion.reflexion(normal), escena, profundidad - 1);

	// r va a estar sí o sí adentro del mismo objeto que yo
	if (adentro != nullptr)
		r.adentro_de(adentro);

	return objeto->luz_especular() * r.trazar(ModoRender::Completo) * 0.5;
}

optional<Vector> Rayo::aplicar_snell(const Vector &v, const Vector &n, float nabla1, float nabla2) {
	// Calcular el ángulo de incidencia
	float theta1 = n.angulo(v);
	float theta_critico = std::asin(nabla2 / nabla1);

	if (nabla1 > nabla2 && theta1 > theta_critico)
		return std::nullopt;

	float theta2 = std::asin(std::sin(theta1) * nabla1 / nabla2);

	return v.cambiar_angulo(n, theta2);
}

Color Rayo::transparencia(const Objeto *objeto, const Vector &punto, const Vector &normal) const {
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
		Rayo t(punto + direccion * (1e-4 + EPSILON), *direccion_salida, escena, profundidad - 1);

		if (objeto != adentro)
			t.adentro_de(objeto); // Estamos entrando al objeto, especificarlo

		return objeto->luz_difusa() * t.trazar(ModoRender::Completo);
	}

	return {0, 0, 0};
}

Color Rayo::sombra(const Objeto *objeto, float d) const {
	Vector punto = punto_inicial + d * direccion;
	Vector normal = objeto->normal_en_punto(punto);

	Color color = objeto->luz_ambiente() * escena.luz_ambiente();

	for (const auto luz : escena.luces()) {
		Vector direccion_a_luz = luz->get_posicion() - punto;
		float distancia_luz = direccion_a_luz.get_norma();
		direccion_a_luz = direccion_a_luz / distancia_luz;

		float producto = normal * direccion_a_luz;
		Rayo l(punto, direccion_a_luz, escena, 0);

		// Solo calcular iluminación si la superficie apunta en dirección
		// a la fuente de luz.
		if (producto > 0) {
			Color oclusion_luz = l.oclusion(distancia_luz);

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

pair<const Objeto *, float> Rayo::objeto_mas_cercano() const {
	float distancia_minima = numeric_limits<float>::infinity();
	const Objeto *mas_cercano = nullptr;

	for (auto objeto : escena.objetos()) {
		float distancia = objeto->interseccion_mas_cercana(punto_inicial, direccion);

		if (distancia < distancia_minima && distancia > EPSILON) {
			mas_cercano = objeto;
			distancia_minima = distancia;
		}
	}

	return {mas_cercano, distancia_minima - EPSILON};
}

void Rayo::adentro_de(const Objeto *o) {
	adentro = o;
}

Rayo::Rayo(const Vector punto_inicial, const Vector direccion,
	 const Escena &escena, int profundidad)
	: punto_inicial(punto_inicial), direccion(direccion),
	  escena(escena), adentro(nullptr), profundidad(profundidad) {}

Color Rayo::trazar(ModoRender modo) const {
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
