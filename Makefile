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
