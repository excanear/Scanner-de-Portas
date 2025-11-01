#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "scanner.h"

void print_usage(const char *prog) {
    printf("Usage: %s <host> <start>-<end> [timeout_ms] [threads]\n", prog);
    printf("Example: %s 127.0.0.1 1-1024 200 50\n", prog);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    const char *host = argv[1];
    const char *range = argv[2];
    int timeout_ms = 200; // default
    int threads = 10;

    if (argc >= 4) timeout_ms = atoi(argv[3]);
    if (argc >= 5) threads = atoi(argv[4]);

    int start = 0, end = 0;
    if (sscanf(range, "%d-%d", &start, &end) != 2) {
        print_usage(argv[0]);
        return 1;
    }

    if (start < 1) start = 1;
    if (end > 65535) end = 65535;
    if (start > end) {
        fprintf(stderr, "Start port must be <= end port\n");
        return 1;
    }

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return 1;
    }

    struct addrinfo hints;
    struct addrinfo *res = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int gai = getaddrinfo(host, NULL, &hints, &res);
    if (gai != 0 || !res) {
        fprintf(stderr, "Erro ao resolver host %s: código %d\n", host, gai);
        WSACleanup();
        return 1;
    }

    scan_opts_t opts;
    memset(&opts, 0, sizeof(opts));
    strncpy(opts.host, host, sizeof(opts.host)-1);
    opts.addr = res; // passamos a primeira lista (resolver retorna lista)
    opts.start_port = start;
    opts.end_port = end;
    opts.timeout_ms = timeout_ms;
    opts.threads = threads;

    printf("Scanning %s ports %d-%d (timeout %d ms, threads %d)\n", host, start, end, timeout_ms, threads);

    int r = run_scan(&opts);

    freeaddrinfo(res);
    WSACleanup();
    return (r == 0) ? 0 : 1;
}
