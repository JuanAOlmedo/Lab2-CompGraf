#include <objetos/cubo.hpp>

Cubo::Cubo(const nlohmann::json& j)
	: Objeto(j),
	  centro(j.at("posicion").get<Vector>()),
	  largo(j.at("largo").get<float>()),
	  alto(j.at("alto").get<float>()),
	  ancho(j.at("ancho").get<float>()) {}

Interseccion Cubo::interseccion_mas_cercana(const Vector &p, const Vector &v) const {

}

