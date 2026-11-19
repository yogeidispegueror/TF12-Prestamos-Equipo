#pragma once
#include <string>

struct Equipo {
    std::string codigo;
    std::string descripcion;
    std::string tipo;
};

struct Usuario {
    std::string codigo;
    std::string nombre;
};

enum class EstadoPrestamo { ACTIVO, DEVUELTO };

struct Prestamo {
    std::string codigo;
    std::string codigoEquipo;
    std::string codigoUsuario;
    EstadoPrestamo estado = EstadoPrestamo::ACTIVO;
};

inline std::string estadoATexto(EstadoPrestamo e) {
    return (e == EstadoPrestamo::ACTIVO) ? "ACTIVO" : "DEVUELTO";
}

inline bool textoAEstado(const std::string& s, EstadoPrestamo& out) {
    if (s == "ACTIVO") { out = EstadoPrestamo::ACTIVO;   return true; }
    if (s == "DEVUELTO") { out = EstadoPrestamo::DEVUELTO; return true; }
    return false;
}