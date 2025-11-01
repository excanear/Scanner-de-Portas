// scanner.h
#ifndef SCANNER_H
#define SCANNER_H

#include <winsock2.h>
#include <ws2tcpip.h>

typedef struct {
    char host[256];
    struct addrinfo *addr; // resolved address (first result)
    int start_port;
    int end_port;
    int timeout_ms;
    int threads;
} scan_opts_t;

// retorna: 1 = aberto, 0 = fechado, -1 = timeout/indeterminado/erro
int scan_port(struct addrinfo *addr, int port, int timeout_ms);

// executa o scan de acordo com opções e imprime resultados
int run_scan(scan_opts_t *opts);

#endif // SCANNER_H
