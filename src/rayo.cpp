#include <rayo.hpp>

Color Rayo::oclusion(float distancia_maxima) const {
	Color color({255, 255, 255});

	for (const auto objeto : escena.objetos()) {
		Interseccion inter = objeto->interseccion_mas_cercana(punto_inicial, direccion);

		if (inter.d < distancia_maxima) {
			const Material material = objeto->get_material();

			// Si nos chocamos contra un objeto opcao, terminar y devolver el color
			// negro
			if (material.transparencia == 1)
				return {0, 0, 0};

			color *= material.difusa * (1 / (5 * material.transparencia));
		}
	}

	return color;
}

Color Rayo::reflexion(const Vector &punto, const Vector &normal) const {
	Rayo r(punto, direccion.reflexion(normal), escena, profundidad - 1);

	// r va a estar sí o sí adentro del mismo objeto que yo
	if (adentro != nullptr)
		r.adentro_de(adentro);

	return r.trazar(ModoRender::Completo);
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
		nabla1 = adentro->get_material().refractividad;

	if (objeto == adentro) {
		// Si estamos adentro del objeto que intersecamos, el segundo índice de
		// refracción es el aire.
		nabla2 = INDICE_REFRACCION_AIRE;
		normal_efectiva = normal;
	} else {
		// Si no, el segundo índice de refracción es el del objeto intersecado.
		nabla2 = objeto->get_material().refractividad;
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

		return t.trazar(ModoRender::Completo);
	}

	return {0, 0, 0};
}

Color Rayo::sombra(Interseccion inter) const {
	const Objeto *objeto = inter.objeto;
	Vector punto = punto_inicial + inter.d * direccion;
	Vector normal = inter.normal;
	const Material material = objeto->get_material();

	Color color = material.ambiente * escena.luz_ambiente();

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
			color += oclusion_luz * material.difusa * luz->luz_difusa() * producto;

			// Calcular luz especular
			Vector h = (direccion_a_luz - direccion).normal();
			float producto_especular = powf(normal * h, COEFICIENTE_ESPECULAR);

			color += oclusion_luz * material.especular * luz->luz_especular() * producto_especular;
		}

		// Calcular componentes de reflexión y transparencia si corresponde,
		if (profundidad > 0) {
			if (material.reflectividad > 0)
				color += reflexion(punto, normal) * material.especular * material.reflectividad;

			if (material.transparencia < 1)
				color += transparencia(objeto, punto, normal) * material.difusa;
		}
	}

	return color;
}

Interseccion Rayo::objeto_mas_cercano() const {
	Interseccion inter_minima = {nullptr, numeric_limits<float>::infinity(), Vector()};

	for (auto objeto : escena.objetos()) {
		auto inter = objeto->interseccion_mas_cercana(punto_inicial, direccion);

		if (inter.d < inter_minima.d && inter.d > EPSILON) {
			inter_minima = inter;
		}
	}

	inter_minima.d -= EPSILON;
	return inter_minima;
}

void Rayo::adentro_de(const Objeto *o) {
	adentro = o;
}

Rayo::Rayo(const Vector punto_inicial, const Vector direccion,
	 const Escena &escena, int profundidad)
	: punto_inicial(punto_inicial), direccion(direccion),
	  escena(escena), adentro(nullptr), profundidad(profundidad) {}

Color Rayo::trazar(ModoRender modo) const {
	Interseccion inter = objeto_mas_cercano();

	if (inter.objeto != nullptr) {
		Color blanco({255, 255, 255});

		if (modo == ModoRender::SoloReflexion)
			return blanco * inter.objeto->get_material().reflectividad;
		else if (modo == ModoRender::SoloTransparencia)
			return blanco * (1 - inter.objeto->get_material().transparencia);
		else
			return sombra(inter);
	} else
		return escena.color_fondo();
}
