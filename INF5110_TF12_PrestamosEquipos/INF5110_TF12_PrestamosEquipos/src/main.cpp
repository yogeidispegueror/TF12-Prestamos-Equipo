// https://github.com/yogeidispegueror/TF12-Prestamos-Equipo

#include <iostream>
#include <limits>
#include <string>
#include "Modelos.h"
#include "Equipos.h"
#include "Persistencia.h"

namespace {
    const std::string RUTA = "data/equipos.txt";

    std::string pedirLinea(const std::string& etiqueta) {
        std::cout << etiqueta;
        std::string s;
        if (!std::getline(std::cin, s)) {
            std::cin.clear();
            s.clear();
        }
        return s;
    }

    int pedirOpcion() {
        std::string s = pedirLinea("Opcion: ");
        try { return std::stoi(trim(s)); }
        catch (...) { return -1; }
    }

    void listarDisponibles(const std::vector<Equipo>& equipos, const std::vector<Prestamo>& prestamos) {
        if (equipos.empty()) {
            std::cout << "Todavia no se han registrado equipos.\n";
            return;
        }
        bool hay = false;
        std::cout << "\nEquipos disponibles:\n";
        for (const auto& e : equipos) {
            if (equipoEstaDisponible(prestamos, e.codigo)) {
                std::cout << "  " << e.codigo << " | " << e.descripcion << " | " << e.tipo << '\n';
                hay = true;
            }
        }
        if (!hay) std::cout << "  (No hay equipos disponibles.)\n";
    }

    void mostrarResumen(const std::vector<Equipo>& equipos, const std::vector<Prestamo>& prestamos) {
        int disp = contarDisponibles(equipos, prestamos);
        int asig = contarAsignados(equipos, prestamos);
        std::cout << "Total equipos: " << equipos.size() << " | Disponibles: " << disp << " | Asignados: " << asig << '\n';
        auto porTipo = clasificarDisponiblesPorTipo(equipos, prestamos);
        if (!porTipo.empty()) {
            std::cout << "Disponibles por tipo:\n";
            for (const auto& par : porTipo)
                std::cout << "  " << par.first << ": " << par.second << '\n';
        }
    }

    void mostrarMenu() {
        std::cout << "\n===== TF12 Prestamo de Equipos =====\n"
            << "1) Registrar equipo\n"
            << "2) Registrar usuario\n"
            << "3) Asignar equipo\n"
            << "4) Registrar devolucion\n"
            << "5) Consultar equipos disponibles\n"
            << "6) Ver resumen (conteo y tipos)\n"
            << "7) Guardar datos\n"
            << "0) Salir\n";
    }
}

int main() {
    std::vector<Equipo>   equipos;
    std::vector<Usuario>  usuarios;
    std::vector<Prestamo> prestamos;

    std::string error;
    bool cambiosPendientes = false;

    if (cargarDatos(RUTA, equipos, usuarios, prestamos, error)) {
        std::cout << "Datos cargados (" << equipos.size() << " equipos, " << usuarios.size() << " usuarios, " << prestamos.size() << " prestamos).\n";
    }
    else {
        std::cout << "AVISO: no se pudo cargar " << RUTA << ": " << error << '\n'
            << "Se inicia en modo vacío y NO se sobrescribira el archivo hasta un guardado explicito.\n";
    }

    bool salir = false;
    while (!salir) {
        mostrarMenu();
        int op = pedirOpcion();
        switch (op) {
        case 1: {
            std::string cod = pedirLinea("Codigo del equipo (ej. E001): ");
            std::string des = pedirLinea("Descripcion: ");
            std::string tip = pedirLinea("Tipo (Proyector/Portatil): ");
            if (registrarEquipo(equipos, cod, des, tip, error)) {
                std::cout << "Equipo registrado.\n";
                cambiosPendientes = true;
            }
            else std::cout << "Rechazado: " << error << '\n';
            break;
        }
        case 2: {
            std::string cod = pedirLinea("Codigo del usuario (ej. U001): ");
            std::string nom = pedirLinea("Nombre completo: ");
            if (registrarUsuario(usuarios, cod, nom, error)) {
                std::cout << "Usuario registrado.\n";
                cambiosPendientes = true;
            }
            else std::cout << "Rechazado: " << error << '\n';
            break;
        }
        case 3: {
            std::string ce = pedirLinea("Codigo del equipo: ");
            std::string cu = pedirLinea("Codigo del usuario: ");
            std::string nuevo;
            if (asignarEquipo(prestamos, equipos, usuarios, ce, cu, nuevo, error)) {
                std::cout << "Prestamo creado: " << nuevo << '\n';
                cambiosPendientes = true;
            }
            else std::cout << "Rechazado: " << error << '\n';
            break;
        }
        case 4: {
            std::string cp = pedirLinea("Codigo del prestamo a devolver: ");
            if (registrarDevolucion(prestamos, cp, error)) {
                std::cout << "Devolucion registrada.\n";
                cambiosPendientes = true;
            }
            else std::cout << "Rechazado: " << error << '\n';
            break;
        }
        case 5:
            listarDisponibles(equipos, prestamos);
            break;
        case 6:
            mostrarResumen(equipos, prestamos);
            break;
        case 7: {
            if (guardarDatos(RUTA, equipos, usuarios, prestamos, error)) {
                std::cout << "Datos guardados en " << RUTA << '\n';
                cambiosPendientes = false;
            }
            else std::cout << "Error al guardar: " << error << '\n';
            break;
        }
        case 0:
            if (cambiosPendientes) {
                std::string r = pedirLinea("Hay cambios sin guardar. ¿Guardar antes de salir? (s/n): ");
                if (!r.empty() && (r[0] == 's' || r[0] == 'S')) {
                    if (guardarDatos(RUTA, equipos, usuarios, prestamos, error)) {
                        std::cout << "Guardado.\n";
                    }
                    else {
                        std::cout << "No se pudo guardar: " << error << "\nSe cancela la salida.\n";
                        break;
                    }
                }
            }
            salir = true;
            break;
        default:
            std::cout << "Opcion no valida. Intenta de nuevo.\n";
            break;
        }
    }
    std::cout << "Hasta luego.\n";
    return 0;
}
