#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

void print_usage(const char *prog) {
    printf("Usage: %s <host> <start>-<end> [timeout_ms] [threads]\n", prog);
    printf("Example: %s 127.0.0.1 1-1024 200 50\n", prog);
}

int scan_port_simple(const char *host, int port, int timeout_ms) {
    SOCKET s = INVALID_SOCKET;
    struct sockaddr_in addr;
    int result = -1;

    // Resolve host
    struct hostent *he = gethostbyname(host);
    if (!he) return -1;

    // Create socket
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) return -1;

    // Setup address
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)port);
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);

    // Non-blocking mode
    u_long mode = 1;
    ioctlsocket(s, FIONBIO, &mode);

    // Try to connect
    int c = connect(s, (struct sockaddr*)&addr, sizeof(addr));
    if (c == 0) {
        result = 1; // Connected immediately
        closesocket(s);
        return result;
    }

    int err = WSAGetLastError();
    if (err != WSAEWOULDBLOCK) {
        result = 0; // Connection refused
        closesocket(s);
        return result;
    }

    // Use select for timeout
    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(s, &wfds);

    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int sel = select(0, NULL, &wfds, NULL, &tv);
    if (sel <= 0) {
        result = -1; // timeout or error
    } else {
        // Check SO_ERROR
        int so_error = 0;
        int len = sizeof(so_error);
        if (getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&so_error, &len) == 0) {
            result = (so_error == 0) ? 1 : 0;
        } else {
            result = -1;
        }
    }

    closesocket(s);
    return result;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    const char *host = argv[1];
    const char *range = argv[2];
    int timeout_ms = (argc >= 4) ? atoi(argv[3]) : 200;

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

    // Initialize Winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return 1;
    }

    printf("Scanning %s ports %d-%d (timeout %d ms)\n", host, start, end, timeout_ms);

    int open_count = 0;
    for (int port = start; port <= end; port++) {
        int r = scan_port_simple(host, port, timeout_ms);
        if (r == 1) {
            printf("%d/tcp open\n", port);
            open_count++;
        }
        // Only print closed/filtered if explicitly requested (keep output clean)
    }

    printf("\nScan complete. Found %d open ports.\n", open_count);

    WSACleanup();
    return 0;
}