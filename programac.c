#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

// ==========================================
// BLOQUE 1: MACROS Y ESTRUCTURAS
// ==========================================
#define MAX_USUARIOS 20
#define MAX_STR 16 // 15 caracteres + '\0'

typedef enum { BLOQUEADO = 0, ACTIVO = 1 } EstadoUsuario;

typedef struct {
    int id;
    char usuario[MAX_STR];
    char nombreCompleto[MAX_STR * 2];
    char grupo[MAX_STR];
    char password[MAX_STR];
    EstadoUsuario estado;
} Usuario;

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

// Función auxiliar para encontrar usuario por su username (Punteros puros)
Usuario* buscarPorUsuario(Usuario *lista, int total, const char *usernameBuscado) {
    Usuario *p = lista;
    for (int i = 0; i < total; i++) {
        if (strcmp(p->usuario, usernameBuscado) == 0) {
            return p; // Retorna la dirección de memoria exacta donde está el usuario
        }
        p++;
    }
    return NULL; // No encontrado
}

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

// ==========================================
// BLOQUE 4: LÓGICA CRUD (NEGOCIO)
// ==========================================
void crearUsuario(Usuario *lista, int *total) {
    if (*total >= MAX_USUARIOS) {
        printf("[!] Capacidad máxima (%d) alcanzada.\n", MAX_USUARIOS);
        return;
    }

    Usuario *nuevo = lista + (*total);
    nuevo->id = (*total) > 0 ? (lista + (*total - 1))->id + 1 : 1; // ID auto-incremental
    nuevo->estado = ACTIVO;

    printf("Ingrese usuario (max 15 chars): ");
    fgets(nuevo->usuario, MAX_STR, stdin); limpiarSaltoDeLinea(nuevo->usuario);

    printf("Ingrese nombre completo: ");
    fgets(nuevo->nombreCompleto, MAX_STR * 2, stdin); limpiarSaltoDeLinea(nuevo->nombreCompleto);

    printf("Ingrese grupo: ");
    fgets(nuevo->grupo, MAX_STR, stdin); limpiarSaltoDeLinea(nuevo->grupo);

    printf("Ingrese contraseña: ");
    fgets(nuevo->password, MAX_STR, stdin); limpiarSaltoDeLinea(nuevo->password);

    (*total)++;
    printf("\n[+] Usuario %s creado (ID: %d).\n", nuevo->usuario, nuevo->id);
    registrarLog("CREAR", nuevo->usuario);
}

void listarUsuarios(Usuario *lista, int total) {
    if (total == 0) {
        printf("[i] No hay usuarios registrados.\n"); return;
    }
    printf("\n%-5s | %-15s | %-20s | %-15s | %-10s\n", "ID", "USUARIO", "NOMBRE", "GRUPO", "ESTADO");
    printf("--------------------------------------------------------------------------------\n");

    Usuario *p = lista;
    for (int i = 0; i < total; i++) {
        printf("%-5d | %-15s | %-20s | %-15s | %-10s\n", 
            p->id, p->usuario, p->nombreCompleto, p->grupo, 
            (p->estado == ACTIVO ? "ACTIVO" : "BLOQUEADO"));
        p++;
    }
}

void modificarUsuario(Usuario *lista, int total) {
    char busqueda[MAX_STR];
    printf("Ingrese el 'usuario' a modificar: ");
    fgets(busqueda, MAX_STR, stdin); limpiarSaltoDeLinea(busqueda);

    Usuario *obj = buscarPorUsuario(lista, total, busqueda);
    if (!obj) {
        printf("[!] Usuario no encontrado.\n"); return;
    }

    printf("Modificando a: %s\n", obj->usuario);
    printf("Nuevo nombre completo (Enter para omitir): ");
    char temp[MAX_STR * 2];
    fgets(temp, sizeof(temp), stdin); limpiarSaltoDeLinea(temp);
    if (strlen(temp) > 0) strcpy(obj->nombreCompleto, temp);

    printf("Nuevo grupo (Enter para omitir): ");
    fgets(temp, MAX_STR, stdin); limpiarSaltoDeLinea(temp);
    if (strlen(temp) > 0) strcpy(obj->grupo, temp);

    printf("Nueva contraseña (Enter para omitir): ");
    fgets(temp, MAX_STR, stdin); limpiarSaltoDeLinea(temp);
    if (strlen(temp) > 0) strcpy(obj->password, temp);

    printf("[+] Usuario modificado.\n");
    registrarLog("MODIFICAR", obj->usuario);
}

void bloquearDesbloquear(Usuario *lista, int total) {
    char busqueda[MAX_STR];
    printf("Ingrese el 'usuario' a bloquear/desbloquear: ");
    fgets(busqueda, MAX_STR, stdin); limpiarSaltoDeLinea(busqueda);

    Usuario *obj = buscarPorUsuario(lista, total, busqueda);
    if (!obj) {
        printf("[!] Usuario no encontrado.\n"); return;
    }

    if (obj->estado == ACTIVO) {
        obj->estado = BLOQUEADO;
        printf("[+] Usuario %s BLOQUEADO.\n", obj->usuario);
        registrarLog("BLOQUEAR", obj->usuario);
    } else {
        obj->estado = ACTIVO;
        printf("[+] Usuario %s DESBLOQUEADO.\n", obj->usuario);
        registrarLog("DESBLOQUEAR", obj->usuario);
    }
}

void eliminarUsuario(Usuario *lista, int *total) {
    char busqueda[MAX_STR];
    printf("Ingrese el 'usuario' a eliminar: ");
    fgets(busqueda, MAX_STR, stdin); limpiarSaltoDeLinea(busqueda);

    Usuario *obj = buscarPorUsuario(lista, *total, busqueda);
    if (!obj) {
        printf("[!] Usuario no encontrado.\n"); return;
    }

    printf("¿Está seguro de eliminar a %s? (1=Sí, 0=No): ", obj->usuario);
    int conf = obtenerEnteroSeguro();
    if (conf != 1) {
        printf("[i] Operación cancelada.\n"); return;
    }

    // Guardamos el nombre para el log antes de destruirlo
    char nombreEliminado[MAX_STR];
    strcpy(nombreEliminado, obj->usuario);

    // Desplazamiento de memoria: Sobrescribimos arrastrando hacia atrás
    Usuario *actual = obj;
    Usuario *siguiente = obj + 1;
    Usuario *fin = lista + (*total);
    
    while (siguiente < fin) {
        *actual = *siguiente; // Copia toda la estructura bloque a bloque
        actual++;
        siguiente++;
    }

    (*total)--;
    printf("[+] Usuario eliminado correctamente.\n");
    registrarLog("ELIMINAR", nombreEliminado);
}

// ==========================================
// BLOQUE 5: PERSISTENCIA (ARCHIVOS)
// ==========================================
void cargarUsuarios(Usuario *lista, int *total) {
    FILE *archivo = fopen("usuarios.dat", "r");
    if (!archivo) return; // Si no existe, inicia vacío

    char linea[200];
    fgets(linea, sizeof(linea), archivo); // Leer cabecera y descartar

    Usuario *p = lista;
    while (fgets(linea, sizeof(linea), archivo) && *total < MAX_USUARIOS) {
        // Parseo simple de CSV
        char *token = strtok(linea, ","); p->id = atoi(token);
        token = strtok(NULL, ","); strcpy(p->usuario, token);
        token = strtok(NULL, ","); strcpy(p->nombreCompleto, token);
        token = strtok(NULL, ","); strcpy(p->grupo, token);
        token = strtok(NULL, ","); strcpy(p->password, token);
        token = strtok(NULL, "\n"); p->estado = atoi(token);
        
        p++;
        (*total)++;
    }
    fclose(archivo);
}

void guardarUsuarios(Usuario *lista, int total) {
    FILE *archivo = fopen("usuarios.dat", "w");
    if (!archivo) {
        printf("[!] Error al guardar la base de datos.\n"); return;
    }

    fprintf(archivo, "ID,usuario,nombreCompleto,grupo,password,estado\n");
    Usuario *p = lista;
    for (int i = 0; i < total; i++) {
        fprintf(archivo, "%d,%s,%s,%s,%s,%d\n", 
                p->id, p->usuario, p->nombreCompleto, p->grupo, p->password, p->estado);
        p++;
    }
    fclose(archivo);
    printf("[i] Base de datos guardada en usuarios.dat.\n");
}

// ==========================================
// MAIN: FLUJO Y UI
// ==========================================
int main() {
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