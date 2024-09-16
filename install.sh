#!/bin/bash

check_dependency() {
    for cmd in "$@"; do
        command -v "$cmd" >/dev/null 2>&1
        if [[ $? -ne 0 ]]; then
            echo "Error: $cmd no está instalado. Instalando..."
            sudo dnf install -y "$cmd" || { echo "Error: No se pudo instalar $cmd. Saliendo..."; exit 1; }
        else
            echo "$cmd ya está instalado."
        fi
    done
}

run_makefile() {
    echo "Ejecutando Makefile..."
    make || { echo "Error al ejecutar make. Saliendo..."; exit 1; }
}

echo "Verificando dependencias..."
check_dependency "gcc" "gdb" "make" "cmake"

echo "Todas las dependencias están satisfechas."

echo "Creando directorios de salida..."
DIRECTORIES=("output/bin" "output/temp" "output/objects")

for dir in "${DIRECTORIES[@]}"; do
    if [[ ! -d "$dir" ]]; then
        mkdir -p "$dir"
        echo "Directorio $dir creado."
    else
        echo "Directorio $dir ya existe."
    fi
done

run_makefile

echo "¡Programa listo para ejecutar!"
