#!/bin/bash

echo "Compilando..."

gcc main.c -o sistema_teste \
    -I./libs/include \
    ./libs/libiup.a \
    $(pkg-config --cflags --libs gtk+-unix-print-3.0) \
    -lm -lX11 \
    -no-pie

if [ $? -eq 0 ]; then
    echo "Sucesso! Executando..."
    ./sistema_teste
else
    echo "Erro na compilação."
fi