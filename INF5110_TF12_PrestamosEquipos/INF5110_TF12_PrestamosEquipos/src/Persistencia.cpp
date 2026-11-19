#include "Persistencia.h"
#include "Equipos.h"
#include <fstream>
#include <sstream>
#include <cstdio>

namespace {
    std::vector<std::string> splitPipe(const std::string& linea) {
        std::vector<std::string> campos;
        std::string actual;
        for (char c : linea) {
            if (c == '|') { campos.push_back(actual); actual.clear(); }
            else actual.push_back(c);
        }
        campos.push_back(actual);
        return campos;
    }
}

bool cargarDatos(const std::string& ruta, std::vector<Equipo>& equipos, std::vector<Usuario>& usuarios, std::vector<Prestamo>& prestamos, std::string& error) {
    std::ifstream in(ruta);
    if (!in.is_open()) {
        equipos.clear(); usuarios.clear(); prestamos.clear();
        return true;
    }

    std::vector<Equipo>   eqTmp;
    std::vector<Usuario>  usTmp;
    std::vector<Prestamo> prTmp;

    std::string linea;
    int numLinea = 0;
    enum Seccion { NINGUNA, EQ, US, PR } seccion = NINGUNA;
    bool cabeceraVista = false;

    auto errorEn = [&](const std::string& msg) {
        error = "Línea " + std::to_string(numLinea) + ": " + msg;
        return false;
        };

    while (std::getline(in, linea)) {
        ++numLinea;
        if (trim(linea).empty()) continue;

        if (!cabeceraVista) {
            if (linea != "VERSION|1") return errorEn("cabecera esperada 'VERSION|1'");
            cabeceraVista = true;
            continue;
        }
        if (linea == "[EQUIPOS]") { seccion = EQ; continue; }
        if (linea == "[USUARIOS]") { seccion = US; continue; }
        if (linea == "[PRESTAMOS]") { seccion = PR; continue; }

        auto campos = splitPipe(linea);
        if (seccion == EQ) {
            if (campos.size() != 3) return errorEn("equipo requiere 3 campos");
            if (campos[0].empty() || esVacioOSoloEspacios(campos[1]) || esVacioOSoloEspacios(campos[2]))
                return errorEn("equipo con campos vacios");
            if (buscarEquipoPorCodigo(eqTmp, campos[0]) >= 0)
                return errorEn("código de equipo duplicado");
            eqTmp.push_back({ aMayusculas(trim(campos[0])), trim(campos[1]), trim(campos[2]) });
        }
        else if (seccion == US) {
            if (campos.size() != 2) return errorEn("usuario requiere 2 campos");
            if (campos[0].empty() || esVacioOSoloEspacios(campos[1]))
                return errorEn("usuario con campos vacios");
            if (buscarUsuarioPorCodigo(usTmp, campos[0]) >= 0)
                return errorEn("codigo de usuario duplicado");
            usTmp.push_back({ aMayusculas(trim(campos[0])), trim(campos[1]) });
        }
        else if (seccion == PR) {
            if (campos.size() != 4) return errorEn("préstamo requiere 4 campos");
            Prestamo p;
            p.codigo = aMayusculas(trim(campos[0]));
            p.codigoEquipo = aMayusculas(trim(campos[1]));
            p.codigoUsuario = aMayusculas(trim(campos[2]));
            if (p.codigo.empty() || p.codigoEquipo.empty() || p.codigoUsuario.empty())
                return errorEn("préstamo con campos vacios");
            if (!textoAEstado(campos[3], p.estado))
                return errorEn("estado no admitido (usa ACTIVO/DEVUELTO)");
            if (buscarPrestamoPorCodigo(prTmp, p.codigo) >= 0)
                return errorEn("codigo de prestamo duplicado");
            prTmp.push_back(p);
        }
        else {
            return errorEn("registro fuera de seccion");
        }
    }

    if (!cabeceraVista) return errorEn("archivo sin cabecera valida");

    for (const auto& p : prTmp) {
        if (buscarEquipoPorCodigo(eqTmp, p.codigoEquipo) < 0) {
            error = "Préstamo " + p.codigo + ": equipo inexistente " + p.codigoEquipo;
            return false;
        }
        if (buscarUsuarioPorCodigo(usTmp, p.codigoUsuario) < 0) {
            error = "Préstamo " + p.codigo + ": usuario inexistente " + p.codigoUsuario;
            return false;
        }
    }

    for (size_t i = 0; i < prTmp.size(); ++i) {
        if (prTmp[i].estado != EstadoPrestamo::ACTIVO) continue;
        for (size_t j = i + 1; j < prTmp.size(); ++j) {
            if (prTmp[j].estado == EstadoPrestamo::ACTIVO && prTmp[i].codigoEquipo == prTmp[j].codigoEquipo) {
                error = "Dos prestamos ACTIVOS del equipo " + prTmp[i].codigoEquipo;
                return false;
            }
        }
    }

    equipos = std::move(eqTmp);
    usuarios = std::move(usTmp);
    prestamos = std::move(prTmp);
    return true;
}

bool guardarDatos(const std::string& ruta, const std::vector<Equipo>& equipos, const std::vector<Usuario>& usuarios, const std::vector<Prestamo>& prestamos, std::string& error) {
    std::string tmp = ruta + ".tmp";
    std::ofstream out(tmp, std::ios::trunc);
    if (!out.is_open()) { error = "No se pudo abrir " + tmp; return false; }

    out << "VERSION|1\n";
    out << "[EQUIPOS]\n";
    for (const auto& e : equipos)
        out << e.codigo << '|' << e.descripcion << '|' << e.tipo << '\n';
    out << "[USUARIOS]\n";
    for (const auto& u : usuarios)
        out << u.codigo << '|' << u.nombre << '\n';
    out << "[PRESTAMOS]\n";
    for (const auto& p : prestamos)
        out << p.codigo << '|' << p.codigoEquipo << '|' << p.codigoUsuario << '|' << estadoATexto(p.estado) << '\n';

    if (!out.good()) { error = "Error durante la escritura."; out.close(); return false; }
    out.close();

    std::remove(ruta.c_str());
    if (std::rename(tmp.c_str(), ruta.c_str()) != 0) {
        error = "No se pudo reemplazar el archivo definitivo.";
        return false;
    }
    return true;
}