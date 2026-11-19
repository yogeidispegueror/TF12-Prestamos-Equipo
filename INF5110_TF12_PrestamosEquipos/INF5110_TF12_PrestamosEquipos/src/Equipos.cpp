#include "Equipos.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>

std::string trim(const std::string& s) {
    size_t i = 0, j = s.size();
    while (i < j && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    while (j > i && std::isspace(static_cast<unsigned char>(s[j - 1]))) --j;
    return s.substr(i, j - i);
}

std::string aMayusculas(const std::string& s) {
    std::string r = s;
    for (char& c : r) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return r;
}

bool esVacioOSoloEspacios(const std::string& s) {
    return trim(s).empty();
}

bool contieneBarraVertical(const std::string& s) {
    return s.find('|') != std::string::npos;
}

int buscarEquipoPorCodigo(const std::vector<Equipo>& equipos, const std::string& codigo) {
    std::string buscado = aMayusculas(trim(codigo));
    for (size_t i = 0; i < equipos.size(); ++i)
        if (aMayusculas(equipos[i].codigo) == buscado) return static_cast<int>(i);
    return -1;
}

int buscarUsuarioPorCodigo(const std::vector<Usuario>& usuarios, const std::string& codigo) {
    std::string buscado = aMayusculas(trim(codigo));
    for (size_t i = 0; i < usuarios.size(); ++i)
        if (aMayusculas(usuarios[i].codigo) == buscado) return static_cast<int>(i);
    return -1;
}

int buscarPrestamoPorCodigo(const std::vector<Prestamo>& prestamos, const std::string& codigo) {
    std::string buscado = aMayusculas(trim(codigo));
    for (size_t i = 0; i < prestamos.size(); ++i)
        if (aMayusculas(prestamos[i].codigo) == buscado) return static_cast<int>(i);
    return -1;
}

bool equipoEstaDisponible(const std::vector<Prestamo>& prestamos, const std::string& codigoEquipo) {
    for (const auto& p : prestamos)
        if (p.estado == EstadoPrestamo::ACTIVO && aMayusculas(p.codigoEquipo) == aMayusculas(codigoEquipo))
            return false;
    return true;
}

int contarDisponibles(const std::vector<Equipo>& equipos, const std::vector<Prestamo>& prestamos) {
    int n = 0;
    for (const auto& e : equipos)
        if (equipoEstaDisponible(prestamos, e.codigo)) ++n;
    return n;
}

int contarAsignados(const std::vector<Equipo>& equipos, const std::vector<Prestamo>& prestamos) {
    return static_cast<int>(equipos.size()) - contarDisponibles(equipos, prestamos);
}

bool registrarEquipo(std::vector<Equipo>& equipos, const std::string& codigo, const std::string& descripcion, const std::string& tipo, std::string& error) {
    std::string cod = aMayusculas(trim(codigo));
    if (cod.empty()) { error = "El codigo del equipo no puede estar vacio."; return false; }
    if (esVacioOSoloEspacios(descripcion)) { error = "La descripcion no puede estar vacia."; return false; }
    if (esVacioOSoloEspacios(tipo)) { error = "El tipo no puede estar vacio."; return false; }
    if (contieneBarraVertical(descripcion) || contieneBarraVertical(tipo)) {
        error = "Descripcion y tipo no pueden contener el caracter '|'.";
        return false;
    }
    if (buscarEquipoPorCodigo(equipos, cod) >= 0) {
        error = "Ya existe un equipo con el codigo " + cod + ".";
        return false;
    }
    equipos.push_back({ cod, trim(descripcion), trim(tipo) });
    return true;
}

bool registrarUsuario(std::vector<Usuario>& usuarios, const std::string& codigo, const std::string& nombre, std::string& error) {
    std::string cod = aMayusculas(trim(codigo));
    if (cod.empty()) { error = "El codigo del usuario no puede estar vacio."; return false; }
    if (esVacioOSoloEspacios(nombre)) { error = "El nombre no puede estar vacio."; return false; }
    if (contieneBarraVertical(nombre)) { error = "El nombre no puede contener el caracter '|'."; return false; }
    if (buscarUsuarioPorCodigo(usuarios, cod) >= 0) {
        error = "Ya existe un usuario con el codigo " + cod + ".";
        return false;
    }
    usuarios.push_back({ cod, trim(nombre) });
    return true;
}

bool asignarEquipo(std::vector<Prestamo>& prestamos, const std::vector<Equipo>& equipos, const std::vector<Usuario>& usuarios, const std::string& codigoEquipo, const std::string& codigoUsuario, std::string& codigoPrestamoGenerado, std::string& error) {
    if (buscarEquipoPorCodigo(equipos, codigoEquipo) < 0) {
        error = "No existe el equipo con codigo " + trim(codigoEquipo) + ".";
        return false;
    }
    if (buscarUsuarioPorCodigo(usuarios, codigoUsuario) < 0) {
        error = "No existe el usuario con codigo " + trim(codigoUsuario) + ".";
        return false;
    }
    if (!equipoEstaDisponible(prestamos, codigoEquipo)) {
        error = "El equipo ya tiene un prestamo activo.";
        return false;
    }
    Prestamo p;
    p.codigo = siguienteCodigoPrestamo(prestamos);
    p.codigoEquipo = aMayusculas(trim(codigoEquipo));
    p.codigoUsuario = aMayusculas(trim(codigoUsuario));
    p.estado = EstadoPrestamo::ACTIVO;
    prestamos.push_back(p);
    codigoPrestamoGenerado = p.codigo;
    return true;
}

bool registrarDevolucion(std::vector<Prestamo>& prestamos, const std::string& codigoPrestamo, std::string& error) {
    int idx = buscarPrestamoPorCodigo(prestamos, codigoPrestamo);
    if (idx < 0) { error = "No existe el prestamo " + trim(codigoPrestamo) + "."; return false; }
    if (prestamos[idx].estado == EstadoPrestamo::DEVUELTO) {
        error = "Ese prestamo ya fue devuelto; no se puede repetir.";
        return false;
    }
    prestamos[idx].estado = EstadoPrestamo::DEVUELTO;
    return true;
}

std::vector<std::pair<std::string, int>> clasificarDisponiblesPorTipo(const std::vector<Equipo>& equipos, const std::vector<Prestamo>& prestamos) {
    std::vector<std::pair<std::string, int>> resultado;
    for (const auto& e : equipos) {
        if (!equipoEstaDisponible(prestamos, e.codigo)) continue;
        bool encontrado = false;
        for (auto& par : resultado) {
            if (aMayusculas(par.first) == aMayusculas(e.tipo)) { ++par.second; encontrado = true; break; }
        }
        if (!encontrado) resultado.push_back({ e.tipo, 1 });
    }
    return resultado;
}

std::string siguienteCodigoPrestamo(const std::vector<Prestamo>& prestamos) {
    int maxNum = 0;
    for (const auto& p : prestamos) {
        if (p.codigo.size() >= 2 && (p.codigo[0] == 'P' || p.codigo[0] == 'p')) {
            try {
                int n = std::stoi(p.codigo.substr(1));
                if (n > maxNum) maxNum = n;
            }
            catch (...) {}
        }
    }
    std::ostringstream oss;
    oss << "P" << std::setw(3) << std::setfill('0') << (maxNum + 1);
    return oss.str();
}