#!/bin/bash

# Function to check if a command is available
check_dependency() {
    command -v "$1" >/dev/null 2>&1 || { echo >&2 "Error: $1 is required but not installed. Installing..."; sudo dnf install -y "$1"; }
}

# Function to run Makefile
run_makefile() {
    echo "Running Makefile..."
    make 
}

# Check for dependencies
echo "Checking dependencies..."

# Check for gcc, gdb, make, cmake
check_dependency "gcc"
check_dependency "gdb"
check_dependency "make"
check_dependency "cmake"

echo "Dependencies are satisfied."

# Create directories
echo "Creating directories..."
mkdir -p output/bin output/temp output/objects
echo "Directories created successfully."

# Verificar si existen archivos .bin en la carpeta output/temp y eliminarlos
if ls output/temp/*.bin 1> /dev/null 2>&1; then
    echo "Eliminando archivos .bin en output/temp..."f
    rm output/temp/*.bin
else
    echo "No se encontraron archivos .bin en output/temp."
fi

# Run Makefile
run_makefile

echo "Program ready to run!"

# Menu for selecting which program to run
while true; do
    
    echo "--------------------------------"
    echo "Selecciona la opción a ejecutar:"
    echo "1. fork"
    echo "2. serial"
    echo "3. pthread"
    echo "4. Salir"
    echo "--------------------------------"
    
    read -p "Elige una opción (1-4): " option

    case $option in
        1)
            if ls output/temp/*.bin 1> /dev/null 2>&1; then
                  rm output/temp/*.bin
            fi
            clear
            echo "Has seleccionado fork."
            echo "Ejecutando compresor y descompresor en secuencia..."
            echo "--------------------------------"

            # Ejecutar compresor y medir tiempo
            echo "Ejecutando fork_compressor con time:"
            time output/objects/fork_compressor

            # Ejecutar descompresor y medir tiempo
            echo "--------------------------------"
            echo "Ejecutando fork_decompressor con time:"
            time output/objects/fork_decompressor

            ;;
        2)
            clear
            echo "Has seleccionado serial."
            echo "Ejecutando compresor y descompresor en secuencia..."
            echo "--------------------------------"

            # Ejecutar compresor y medir tiempo
            echo "Ejecutando serial_compressor con time:"
            time output/objects/serial_compressor

            # Ejecutar descompresor y medir tiempo
            echo "--------------------------------"
            echo "Ejecutando serial_decompressor con time:"
            time output/objects/serial_decompressor

            ;;
        3)
            clear
            echo "Has seleccionado pthread."
            echo "Ejecutando compresor y descompresor en secuencia..."
            echo "--------------------------------"

            # Ejecutar compresor y medir tiempo
            echo "Ejecutando pthread_compressor con time:"
            time output/objects/pthread_compressor

            # Ejecutar descompresor y medir tiempo
            echo "--------------------------------"
            echo "Ejecutando pthread_decompressor con time:"
            time output/objects/pthread_decompressor

            ;;
        4)
            echo "Saliendo del programa. ¡GRACIAS!"
            break
            ;;
        *)
            echo "Opción inválida. Intenta de nuevo."
            ;;
    esac
done
