#ifndef USUARIOS
#define USUARIOS

Usuario* buscarPorUsuario(Usuario *lista, int total, const char *usernameBuscado)

void crearUsuario(Usuario *lista, int *total)

void listarUsuarios(Usuario *lista, int total)

void modificarUsuario(Usuario *lista, int total)

void bloquearDesbloquear(Usuario *lista, int total)

void eliminarUsuario(Usuario *lista, int *total)

void cargarUsuarios(Usuario *lista, int *total)

void guardarUsuarios(Usuario *lista, int total)

#endif
