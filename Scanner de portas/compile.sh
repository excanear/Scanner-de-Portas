#!/usr/bin/env bash
# ============================================================
# compile.sh - Compila o scanner no MSYS2/MinGW (bash)
# Requer: gcc (MinGW-w64) no PATH.
# Gera: scanner_simple.exe (standalone) e scanner.exe (completo)
# ============================================================
set -euo pipefail

if ! command -v gcc >/dev/null 2>&1; then
    echo "[ERRO] gcc nao encontrado. Instale o MinGW-w64 (pacman -S mingw-w64-x86_64-gcc)." >&2
    exit 1
fi

echo "[*] Compilando versao standalone (scanner_simple.exe)..."
gcc -O2 -Wall scanner_simple.c -o scanner_simple.exe -lws2_32

echo "[*] Compilando versao completa (scanner.exe)..."
gcc -O2 -Wall -I src src/main.c src/scanner.c -o scanner.exe -lws2_32

echo "[OK] Build concluido: scanner_simple.exe e scanner.exe"
