#!/bin/bash

echo "============================================="
echo "Iniciando criacao da estrutura do projeto..."
echo "============================================="

# 1. CRIANDO PASTAS
echo "[+] Criando diretorios..."
mkdir -p libs/IUP_linux
mkdir -p libs/IUP_windows
mkdir -p redist
mkdir -p data/alunos
mkdir -p data/treinos
mkdir -p data/tmp  # Pasta temporária solicitada
mkdir -p data/icons/interface_icons/linux
mkdir -p data/icons/interface_icons/windows
mkdir -p docs/licenses
mkdir -p .vscode

# 2. CRIANDO ARQUIVOS DE CÓDIGO (EM BRANCO/TEMPLATE)
echo "[+] Criando arquivos fonte (.c e .h)..."
touch main_cli.c main_gui.c
touch aluno.c aluno.h
touch treino.c treino.h
touch processamento.c processamento.h
touch util.c util.h

# Criando constantes.h com a lógica de SO já pré-definida
cat <<EOF > constantes.h
#ifndef CONSTANTES_H
#define CONSTANTES_H

#ifdef _WIN32
    // --- WINDOWS ---
    #define PATH_PDF_BUILDER "redist\\\\pdf_build.exe"
    #define PATH_PDF_VIEWER  "redist\\\\pdf_print.exe"
    #define COMANDO_LIMPEZA  "del /Q"
    #define PASTA_TMP        "data\\\\tmp\\\\"
#else
    // --- LINUX ---
    #define PATH_PDF_BUILDER "./redist/pdf_build" 
    #define PATH_PDF_VIEWER  "xdg-open" 
    #define COMANDO_LIMPEZA  "rm -f"
    #define PASTA_TMP        "data/tmp/"
#endif

#endif
EOF

# 3. CRIANDO ARQUIVO DE ÍNDICE
touch data/alunos.idx

# 4. CRIANDO GITIGNORE
echo "[+] Configurando .gitignore..."
cat <<EOF > .gitignore
# Ignorar binários e objetos
*.o
*.exe
*.dll
*.so
*.a
# Manter libs estáticas
!libs/**/*.a
!libs/**/*.so
!libs/**/*.dll

# Ignorar executáveis finais
gerente_cli
gerente_gui

# Ignorar pastas de build/dist
dist/
build/

# Ignorar dados de teste e logs
data/alunos/*.bin
data/treinos/*.bin
data/tmp/*
*.log

# Manter o arquivo de índice vazio se quiser, ou ignorar se for gerado
# !data/alunos.idx

# Ignorar configurações locais do editor (opcional)
.vscode/

# EXCEÇÃO IMPORTANTE: Rastrear ferramentas renomeadas
!redist/pdf_build.exe
!redist/pdf_print.exe
!redist/pdf_build
EOF

# 5. CRIANDO MAKEFILE
echo "[+] Gerando Makefile..."
cat <<'EOF' > Makefile
# ==========================================
# ESTRUTURA DE DIRETÓRIOS
# ==========================================
DIR_LIBS_LINUX = libs/IUP_linux
DIR_LIBS_WIN   = libs/IUP_windows

# Fontes comuns a ambas as versões
SRC_COMUM = aluno.c treino.c processamento.c util.c
OBJ_COMUM = $(SRC_COMUM:.c=.o)

# ==========================================
# COMPILAÇÃO LINUX (Nativa)
# ==========================================
CC_LINUX = gcc
CFLAGS_LINUX = -Wall -std=c99 -I. -I$(DIR_LIBS_LINUX)
# Linkagem: IUP local + GTK do sistema
LDFLAGS_LINUX = -L$(DIR_LIBS_LINUX) -liup -lgtk-3 -lgdk-3 -lcairo -lgobject-2.0 -lm

# ==========================================
# COMPILAÇÃO WINDOWS (Cross-Compile)
# ==========================================
CC_WIN = x86_64-w64-mingw32-gcc
CFLAGS_WIN = -Wall -std=c99 -I. -I$(DIR_LIBS_WIN)
# Linkagem: Static IUP + Libs do Windows
LDFLAGS_WIN = -L$(DIR_LIBS_WIN) -static -liup -lgdi32 -lcomdlg32 -lcomctl32 -luuid -lole32 -mwindows

# ==========================================
# ALVOS (TARGETS)
# ==========================================

all: cli gui

# --- Versão CLI (Terminal) ---
cli: $(OBJ_COMUM) main_cli.o
	@echo "Compilando CLI..."
	$(CC_LINUX) $(CFLAGS_LINUX) -o gerente_cli main_cli.o $(OBJ_COMUM)

# --- Versão GUI (Linux) ---
gui: $(OBJ_COMUM) main_gui.o
	@echo "Compilando GUI Linux..."
	$(CC_LINUX) $(CFLAGS_LINUX) -o gerente_gui main_gui.o $(OBJ_COMUM) $(LDFLAGS_LINUX)

# --- Versão GUI (Windows) ---
windows:
	@echo "Compilando GUI Windows (Cross)..."
	$(CC_WIN) $(CFLAGS_WIN) -o gerente_gui.exe main_gui.c $(SRC_COMUM) $(LDFLAGS_WIN)

# --- Regra de Distribuição ---
dist: all
	@echo "Empacotando..."
	mkdir -p dist/redist dist/data/tmp dist/data/icons
	cp gerente_gui dist/
	cp -r data/icons dist/data/
	-cp redist/pdf_build.exe dist/redist/ 2>/dev/null || :
	-cp redist/pdf_print.exe dist/redist/ 2>/dev/null || :
	-cp redist/pdf_build dist/redist/ 2>/dev/null || :
	@echo "Pronto em 'dist/'"

# --- Limpeza ---
clean:
	rm -f *.o gerente_cli gerente_gui gerente_gui.exe
EOF

# 6. CRIANDO REQUERIMENTOS.TXT
echo "[+] Criando documentation/requerimentos.txt..."
cat <<EOF > requerimentos.txt
==============================================================================
REQUERIMENTOS DE COMPILAÇÃO E SISTEMA
==============================================================================
[1] COMPILAÇÃO (LINUX/DEV)
- GCC 5.0+
- Make 3.8+
- libgtk-3-dev (Para linkar IUP no Linux)
- mingw-w64 (Opcional: Para gerar .exe do Windows)

[2] EXECUÇÃO (LINUX)
- GTK+ 3.0
- Redistribuível: redist/pdf_build (wkhtmltopdf renomeado)

[3] EXECUÇÃO (WINDOWS)
- Windows 7 SP1 ou superior
- Redistribuíveis: 
    - redist/pdf_build.exe (wkhtmltopdf)
    - redist/pdf_print.exe (SumatraPDF)
EOF

# 7. LICENÇAS E CRÉDITOS
echo "[+] Criando arquivos de licença..."

cat <<EOF > LICENSE
MIT License
Copyright (c) 2026 [Seu Nome/Grupo]
(Este é um arquivo de licença padrão, edite conforme necessário)
EOF

cat <<EOF > CREDITS.txt
===========================================================================
CRÉDITOS E LICENÇAS DE TERCEIROS
===========================================================================
Este software utiliza componentes de terceiros renomeados para padronização.

1. ENGINE DE PDF (redist/pdf_build[.exe])
   - Original: wkhtmltopdf
   - Licença: LGPLv3
   - Fonte: https://wkhtmltopdf.org/

2. VISUALIZADOR DE PDF (redist/pdf_print.exe)
   - Original: SumatraPDF
   - Licença: GPLv3
   - Fonte: https://www.sumatrapdfreader.org/

3. BIBLIOTECA GRÁFICA (libs/IUP)
   - Original: IUP (Portable User Interface)
   - Licença: MIT
   - Fonte: https://www.tecgraf.puc-rio.br/iup/
EOF

# 8. CONFIGURAÇÃO VSCODIUM (JSONs)
echo "[+] Configurando VSCodium (.vscode)..."

# c_cpp_properties.json
cat <<EOF > .vscode/c_cpp_properties.json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "\${workspaceFolder}/**",
                "\${workspaceFolder}/libs/IUP_linux/**",
                "\${workspaceFolder}/libs/IUP_windows/**"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "c99",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ],
    "version": 4
}
EOF

# tasks.json
cat <<EOF > .vscode/tasks.json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build CLI",
            "type": "shell",
            "command": "make cli",
            "group": { "kind": "build", "isDefault": true },
            "problemMatcher": ["\$gcc"]
        },
        {
            "label": "Build GUI (Linux)",
            "type": "shell",
            "command": "make gui",
            "group": "build",
            "problemMatcher": ["\$gcc"]
        },
        {
            "label": "Build GUI (Windows)",
            "type": "shell",
            "command": "make windows",
            "group": "build",
            "problemMatcher": ["\$gcc"]
        },
        {
            "label": "Clean",
            "type": "shell",
            "command": "make clean"
        }
    ]
}
EOF

# launch.json
cat <<EOF > .vscode/launch.json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug CLI",
            "type": "cppdbg",
            "request": "launch",
            "program": "\${workspaceFolder}/gerente_cli",
            "cwd": "\${workspaceFolder}",
            "MIMode": "gdb",
            "preLaunchTask": "Build CLI"
        },
        {
            "name": "Debug GUI",
            "type": "cppdbg",
            "request": "launch",
            "program": "\${workspaceFolder}/gerente_gui",
            "cwd": "\${workspaceFolder}",
            "MIMode": "gdb",
            "preLaunchTask": "Build GUI (Linux)"
        }
    ]
}
EOF

# 9. PLACEHOLDERS (Para você lembrar de substituir)
echo "[+] Criando placeholders para os binários (SUBSTITUA DEPOIS)..."
touch redist/pdf_build.exe
touch redist/pdf_print.exe
touch redist/pdf_build
chmod +x redist/pdf_build

echo "============================================="
echo "ESTRUTURA CRIADA COM SUCESSO!"
echo "============================================="
echo "Próximos passos:"
echo "1. Coloque os arquivos da IUP Linux em libs/IUP_linux/"
echo "2. Coloque os arquivos da IUP Windows em libs/IUP_windows/"
echo "3. Substitua os arquivos vazios em redist/ pelos executáveis reais."
echo "4. Comece a codar!"
