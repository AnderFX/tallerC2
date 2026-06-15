#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>


// ==========================================
// BLOQUE 3: AUDITORÍA (LOGS)
// ==========================================
void registrarLog(const char *accion, const char *usuarioAfectado) {
    FILE *log = fopen("transacciones.log", "a");
    if (!log) return;

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char fechaHora[25];
    strftime(fechaHora, 25, "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(log, "%s | admin | %s | %s\n", fechaHora, accion, usuarioAfectado);
    fclose(log);
}

