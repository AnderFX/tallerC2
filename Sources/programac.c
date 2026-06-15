#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define MAX_USUARIOS 20
#define MAX_STR 16 // 15 caracteres + '\0'

// ==========================================
// BLOQUE 2: HERRAMIENTAS DE SEGURIDAD
// ==========================================
void limpiarSaltoDeLinea(char *cadena) {
    size_t len = strlen(cadena);
    if (len > 0 && cadena[len - 1] == '\n') {
        cadena[len - 1] = '\0';
    }
}

int obtenerEnteroSeguro() {
    char buffer[50];
    char *endptr;
    if (!fgets(buffer, sizeof(buffer), stdin)) return -1;
    errno = 0;
    long valor = strtol(buffer, &endptr, 10);
    if (errno != 0 || endptr == buffer || *endptr != '\n') return -1;
    return (int)valor;
}

// ==========================================
// MAIN: FLUJO Y UI
// ==========================================
int main() {

    FILE *archivo;
    const char *ruta = "usuarios.dat";
    char caracter;
    int totalUsuarios = 0;

    archivo = fopen(ruta, "r");

    // Si el archivo usuarios.dat ya se creó
    if (archivo != NULL) {

        while ((caracter = fgetc(archivo)) != EOF) {
            if (caracter == '\n') {
                totalUsuarios++;
            }
        }   

        cargarUsuarios(ruta, &totalUsuarios);
        
        fclose(archivo); // Recuerda cerrarlo siempre que lo abras
    }

    Usuario *listaUsuarios = malloc(MAX_USUARIOS * sizeof(Usuario));
    if (!listaUsuarios) {
        printf("Error fatal: Memoria insuficiente.\n"); return 1;
    }

    int totalUsuarios = 0;
    cargarUsuarios(listaUsuarios, &totalUsuarios);

    int opcion = -1;
    do {
        printf("\n=== GESTIÓN DE USUARIOS LINUX ===\n");
        printf("[1] Crear usuario\n");
        printf("[2] Listar todos los usuarios\n");
        printf("[3] Modificar usuario\n");
        printf("[4] Bloquear / desbloquear usuario\n");
        printf("[5] Eliminar usuario\n");
        printf("[0] Salir y guardar\n");
        printf("Elija una opción: ");
        
        opcion = obtenerEnteroSeguro();

        switch (opcion) {
            case 1: crearUsuario(listaUsuarios, &totalUsuarios); break;
            case 2: listarUsuarios(listaUsuarios, totalUsuarios); break;
            case 3: modificarUsuario(listaUsuarios, totalUsuarios); break;
            case 4: bloquearDesbloquear(listaUsuarios, totalUsuarios); break;
            case 5: eliminarUsuario(listaUsuarios, &totalUsuarios); break;
            case 0: guardarUsuarios(listaUsuarios, totalUsuarios); break;
            default: printf("\n[!] Opción inválida. Ingrese un número del 0 al 5.\n");
        }
    } while (opcion != 0);

    free(listaUsuarios);
    return 0;
}

