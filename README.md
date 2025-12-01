# Tarea 2: Sincronización y Memoria Virtual

Este repositorio contiene la implementación de la Tarea 2, dividida en dos partes fundamentales: primitivas de sincronización (Barrera Reutilizable) y gestión de memoria (Simulador de Memoria Virtual).

## Integrantes
* Matias Tirado Lopez
* Martin Lara Castro

---

## Parte I: Sincronización con Barrera Reutilizable

Implementación de una barrera reutilizable utilizando monitores (`pthread_mutex_t` y `pthread_cond_t`) para coordinar n hebras a través de múltiples etapas.

### Compilación
Para compilar el programa, navegue a la carpeta correspondiente y ejecute:

```bash
gcc -o parte1 main.c -pthread
```
### Ejecución
```bash
./parte1 
```

## Parte II: Simulador de Memoria Virtual (Algoritmo Reloj)
Simulador secuencial que procesa una traza de direcciones virtuales, traduciendo a direcciones físicas mediante paginación simple y reemplazo de páginas usando el algoritmo Reloj.

## Compilación
Para compilar el programa, navegue a la carpeta correspondiente y ejecute:
```bash
gcc -o parte2 sim.c
```
## Ejecución

```bash
./parte2 <N_marcos> <tam_marco> [--verbose] <archivo_trace>
```
## Ejemplo de uso
```bash
./parte2 8 4096 --verbose trace2.txt
```
