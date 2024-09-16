# Compresión de Libros de Gutenberg con el Algoritmo de Huffman

### Proyecto del curso de Sistemas Operativos  
**Instituto Tecnológico de Costa Rica**

## Descripción

Este proyecto implementa el algoritmo de compresión de Huffman en lenguaje C para comprimir los libros del Proyecto Gutenberg. El objetivo es reducir el tamaño de los archivos de texto utilizando técnicas de codificación eficientes y comparar el rendimiento entre diferentes enfoques de implementación.

## Tabla de Contenidos

- [Descripción](#descripción)
- [Características](#características)
- [Requisitos](#requisitos)
- [Estructura del Proyecto](#estructura-del-proyecto)
- [Instalación](#instalación)
- [Uso Automático](#uso-automático)
- [Uso Manual](#uso-manual)
- [Autores](#autores)

## Características

- **Implementación del Algoritmo de Huffman**: Compresión eficiente de archivos de texto.
- **Múltiples Enfoques**:
    - **Serial**: Implementación secuencial básica.
    - **Pthreads**: Uso de hilos para paralelizar tareas.
    - **Fork**: Utilización de procesos para ejecutar operaciones en paralelo.
- **Comparación de Rendimiento**: Análisis del tiempo de ejecución y eficiencia entre las diferentes implementaciones.

## Requisitos

- **gcc**: Compilador de C.
- **gdb**: Depurador de GNU (opcional, para depuración).
- **make**: Herramienta de automatización de compilación.
- **cmake**: Sistema de compilación multiplataforma.
- **Sistema Operativo**: Linux o cualquier otro compatible con herramientas GNU.

## Estructura del Proyecto

```bash
.
├── books
├── output
│   ├── bin
│   ├── books
│   └── temp
└── src
    ├── dependencies
    │   ├── frequency
    │   ├── global
    │   ├── huffman
    │   └── share
    ├── fork
    ├── pthreads
    └── serial        
```

## Instalación

Clonar el repositorio:

```bash
git clone https://github.com/BrandonRetana/IC660-proyecto-1.git
cd IC660-proyecto-1
```

## Uso automático
Esto instalara todas las dependencias necesarias y va a ejecutar una interfaz CLI para utilizar el programa

```bash
./init.sh
```

## Uso Manual
Preparar el entorno:
Asegúrate de tener instaladas todas las herramientas requeridas:

```bash
sudo dnf update
sudo dnf install gcc gdb make cmake
```
Crear la siguiente estructura de carpetas en en la raiz del proyecto al nivel de src

```bash
output
├── bin
├── books
├── objects
└── temp
```
Ejecutar el comando de compilacion:
```bash
make
```
O ejecutar el siguiente script:
```bash
./install.sh
```
Después de la compilación, puedes ejecutar las diferentes versiones del programa:
Implementación Serial:

```bash
./output/objects/serial_compressor
./output/objects/serial_decompressor
```
Implementación con Hilos (Pthreads):

```bash
./output/objects/pthread_compressor
./output/objects/pthread_decompressor
```
Implementación con Procesos (Fork):

```bash
./output/objects/fork_compressor
./output/objects/fork_decompressor
```

Autores
* Brandon Retana Chacón
* Kevin Cubillo Chacón
* Ervin Rodríguez