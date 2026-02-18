ifeq ($(OS),Windows_NT)
    EXT = .exe
    RM = del /Q
    MKDIR = mkdir
    COPY_REDIST = mkdir $(DIST_DIR)\redist & copy redist\*.exe $(DIST_DIR)\redist\ > NUL
else
    EXT =
    RM = rm -rf
    MKDIR = mkdir -p
    COPY_REDIST = mkdir -p $(DIST_DIR)/redist && \
                  cp redist/pdf_build $(DIST_DIR)/redist/ 2>/dev/null || : && \
                  chmod +x $(DIST_DIR)/redist/pdf_build 2>/dev/null || :
endif

CC = gcc
CFLAGS = -Wall -Wextra -I. 

DIST_DIR = dist
OBJS = processamento.o util.o 

all: gerente_cli$(EXT)

gerente_cli$(EXT): main_cli.o $(OBJS)
	$(CC) main_cli.o $(OBJS) -o gerente_cli$(EXT) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

dist: all
	$(RM) $(DIST_DIR)
	$(MKDIR) $(DIST_DIR)/data/alunos $(DIST_DIR)/data/templates $(DIST_DIR)/data/tmp
	$(MKDIR) $(DIST_DIR)/redist
	cp gerente_cli$(EXT) $(DIST_DIR)/
	cp -r data/* $(DIST_DIR)/data/ 2>/dev/null || :
	$(COPY_REDIST)
	@echo "--- DISTRIBUIÇÃO PRONTA ---"

clean:
	$(RM) *.o gerente_cli$(EXT) $(DIST_DIR)
