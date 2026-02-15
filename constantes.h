#ifndef CONSTANTES_H
#define CONSTANTES_H

#ifdef _WIN32
    // --- WINDOWS ---
    #define PATH_PDF_BUILDER "redist\\pdf_build.exe"
    #define PATH_PDF_VIEWER  "redist\\pdf_print.exe"
    #define COMANDO_LIMPEZA  "del /Q"
    #define PASTA_TMP        "data\\tmp\\"
#else
    // --- LINUX ---
    #define PATH_PDF_BUILDER "./redist/pdf_build" 
    #define PATH_PDF_VIEWER  "xdg-open" 
    #define COMANDO_LIMPEZA  "rm -f"
    #define PASTA_TMP        "data/tmp/"
#endif

#endif
