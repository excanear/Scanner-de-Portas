#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// simples fila de portas (inteiros) compartilhada
static int next_port;
static int start_port_global;
static int end_port_global;
static CRITICAL_SECTION cs_next;
static CRITICAL_SECTION cs_print;
static struct addrinfo *global_addr;
static int global_timeout_ms;

int scan_port(struct addrinfo *addr, int port, int timeout_ms) {
    SOCKET s = INVALID_SOCKET;
    int result = -1;

    // Cria socket
    s = socket(addr->ai_family, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        return -1;
    }

    // Preparar endereço com porta correta (modifica cópia local)
    char addrbuf[128];
    memcpy(addrbuf, addr->ai_addr, addr->ai_addrlen);
    struct sockaddr *addrptr = (struct sockaddr *)addrbuf;

    if (addr->ai_family == AF_INET) {
        struct sockaddr_in *in4 = (struct sockaddr_in *)addrptr;
        in4->sin_port = htons((unsigned short)port);
    } else if (addr->ai_family == AF_INET6) {
        struct sockaddr_in6 *in6 = (struct sockaddr_in6 *)addrptr;
        in6->sin6_port = htons((unsigned short)port);
    }

    // Não-bloqueante
    u_long mode = 1;
    ioctlsocket(s, FIONBIO, &mode);

    int c = connect(s, addrptr, addr->ai_addrlen);
    if (c == 0) {
        // conectado imediatamente
        result = 1;
        closesocket(s);
        return result;
    }

    int err = WSAGetLastError();
    if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS && err != WSAEALREADY) {
        // erro imediato: porta fechada ou erro
        result = 0;
        closesocket(s);
        return result;
    }

    // usar select para esperar writability
    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(s, &wfds);

    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int sel = select(0, NULL, &wfds, NULL, &tv);
    if (sel == 0) {
        // timeout
        result = -1;
        closesocket(s);
        return result;
    } else if (sel < 0) {
        result = -1;
        closesocket(s);
        return result;
    }

    // verificar SO_ERROR
    int so_error = 0;
    int len = sizeof(so_error);
    if (getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)&so_error, &len) == 0) {
        if (so_error == 0) {
            result = 1; // aberto
        } else {
            result = 0; // fechado/refused
        }
    } else {
        result = -1;
    }

    closesocket(s);
    return result;
}

static DWORD WINAPI worker_thread(LPVOID lpParam) {
    (void)lpParam;
    while (1) {
        int port = 0;
        EnterCriticalSection(&cs_next);
        if (next_port > end_port_global) {
            LeaveCriticalSection(&cs_next);
            break;
        }
        port = next_port++;
        LeaveCriticalSection(&cs_next);

        int r = scan_port(global_addr, port, global_timeout_ms);

        EnterCriticalSection(&cs_print);
        if (r == 1) {
            printf("%d/tcp open\n", port);
        } else if (r == 0) {
            printf("%d/tcp closed\n", port);
        } else {
            printf("%d/tcp filtered/timeout\n", port);
        }
        LeaveCriticalSection(&cs_print);
    }
    return 0;
}

int run_scan(scan_opts_t *opts) {
    if (!opts || !opts->addr) return -1;

    // inic WSA já deve estar inicializado por quem chamou, mas garantimos aqui
    // (WSAStartup feito no main())

    InitializeCriticalSection(&cs_next);
    InitializeCriticalSection(&cs_print);

    start_port_global = opts->start_port;
    end_port_global = opts->end_port;
    next_port = start_port_global;
    global_addr = opts->addr;
    global_timeout_ms = opts->timeout_ms;

    int threads = opts->threads > 0 ? opts->threads : 1;
    HANDLE *threads_handles = (HANDLE *)malloc(sizeof(HANDLE) * threads);
    if (!threads_handles) return -1;

    for (int i = 0; i < threads; ++i) {
        DWORD tid;
        threads_handles[i] = CreateThread(NULL, 0, worker_thread, NULL, 0, &tid);
        if (!threads_handles[i]) {
            // criar thread falhou: reduzir número de threads
            threads = i;
            break;
        }
    }

    // esperar
    WaitForMultipleObjects(threads, threads_handles, TRUE, INFINITE);

    for (int i = 0; i < threads; ++i) {
        CloseHandle(threads_handles[i]);
    }

    free(threads_handles);
    DeleteCriticalSection(&cs_next);
    DeleteCriticalSection(&cs_print);
    return 0;
}
