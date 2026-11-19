#pragma once
#include <vector>
#include <string>
#include "Modelos.h"

std::string trim(const std::string& s);
std::string aMayusculas(const std::string& s);
bool esVacioOSoloEspacios(const std::string& s);
bool contieneBarraVertical(const std::string& s);

int buscarEquipoPorCodigo(const std::vector<Equipo>& equipos, const std::string& codigo);
int buscarUsuarioPorCodigo(const std::vector<Usuario>& usuarios, const std::string& codigo);
int buscarPrestamoPorCodigo(const std::vector<Prestamo>& prestamos, const std::string& codigo);

bool equipoEstaDisponible(const std::vector<Prestamo>& prestamos, const std::string& codigoEquipo);
int contarDisponibles(const std::vector<Equipo>& equipos, const std::vector<Prestamo>& prestamos);
int contarAsignados(const std::vector<Equipo>& equipos, const std::vector<Prestamo>& prestamos);

bool registrarEquipo(std::vector<Equipo>& equipos, const std::string& codigo, const std::string& descripcion, const std::string& tipo, std::string& error);
bool registrarUsuario(std::vector<Usuario>& usuarios, const std::string& codigo, const std::string& nombre, std::string& error);

bool asignarEquipo(std::vector<Prestamo>& prestamos, const std::vector<Equipo>& equipos, const std::vector<Usuario>& usuarios, const std::string& codigoEquipo, const std::string& codigoUsuario, std::string& codigoPrestamoGenerado, std::string& error);
bool registrarDevolucion(std::vector<Prestamo>& prestamos, const std::string& codigoPrestamo, std::string& error);

std::vector<std::pair<std::string, int>> clasificarDisponiblesPorTipo(const std::vector<Equipo>& equipos, const std::vector<Prestamo>& prestamos);
std::string siguienteCodigoPrestamo(const std::vector<Prestamo>& prestamos);