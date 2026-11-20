# INF-5110 · TF12 · Préstamos de Equipos

Aplicación de consola en C++ para registrar equipos y usuarios, asignar
unidades (préstamos), registrar devoluciones y consultar qué equipos están
disponibles. Los datos se conservan en un archivo de texto y se recuperan al
abrir el programa.

**Asignatura:** Laboratorio de Introducción a la Programación (INF-5110)
**Trabajo:** Final TF12 — Préstamos de equipos (15 puntos)
**Autor:** [TU NOMBRE COMPLETO] — Matrícula: [TU MATRÍCULA]
**Fecha:** [MES AÑO]

## 2. Funcionalidades

| # | Funcionalidad | Qué hace |
|---|---------------|----------|
| 1 | Registrar equipo | Añade una unidad con código único, descripción y tipo. |
| 2 | Registrar usuario | Añade un usuario con código único y nombre. |
| 3 | Asignar equipo | Comprueba que equipo y usuario existan y que el equipo esté disponible antes de crear el préstamo. |
| 4 | Registrar devolución | Localiza un préstamo activo, lo marca como DEVUELTO y libera el equipo. |
| 5 | Consultar disponibles | Lista solo los equipos disponibles, con conteo y clasificación por tipo. |
| 6 | Persistencia | Guarda y recupera equipos, usuarios y préstamos conservando sus relaciones. |

**Menú principal:**

1) Registrar equipo
2) Registrar usuario
3) Asignar equipo (préstamo)
4) Registrar devolución
5) Consultar equipos disponibles
6) Guardar datos
0) Salir

## 3. Estructuras de datos

Definidas en src/Modelos.h:

| Estructura | Campos |
|------------|--------|
| Equipo   | codigo (único), descripcion, tipo (Proyector / Portátil). |
| Usuario  | codigo (único), nombre. |
| Prestamo | codigo (único), codigoEquipo, codigoUsuario, estado (ACTIVO / DEVUELTO). |

Las colecciones se manejan con std::vector (VecEquipos, VecUsuarios, VecPrestamos).

### Identificación
- El código es la identidad del registro (ej. E001, U001, P001).
- Se aplica trim() al registrar, buscar y cargar.
- No se usa la posición del vector como identidad.

### Relaciones
- Prestamo.codigoEquipo → Equipo.codigo
- Prestamo.codigoUsuario → Usuario.codigo
- Un equipo puede tener varios préstamos históricos, pero como máximo uno ACTIVO a la vez.

### Disponibilidad
- contarDisponibles() cuenta equipos sin préstamo activo.
- contarAsignados() = total − disponibles.
- equipoTienePrestamoActivo() recorre los préstamos buscando uno ACTIVO.

### Código de préstamo
Se genera automáticamente como P + número de 3 dígitos (P001, P002…).
Al cargar se reconstruye el siguiente valor con siguienteCodigoPrestamo().

## 4. Reglas adicionales del diseño

- Sin límite de equipos por usuario: un usuario puede tener varios
  préstamos activos de equipos distintos. Prestar dos veces el mismo
  equipo sí se rechaza.
- No se borran registros: la devolución solo cambia el estado a DEVUELTO.
- Un préstamo posterior crea un registro nuevo; no se reactiva el anterior.
- Campos de texto: se rechaza | y saltos de línea. Se conservan espacios interiores.

