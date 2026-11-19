#pragma once
#include <vector>
#include <string>
#include "Modelos.h"

bool cargarDatos(const std::string& ruta, std::vector<Equipo>& equipos, std::vector<Usuario>& usuarios, std::vector<Prestamo>& prestamos, std::string& error);
bool guardarDatos(const std::string& ruta, const std::vector<Equipo>& equipos, const std::vector<Usuario>& usuarios, const std::vector<Prestamo>& prestamos, std::string& error);