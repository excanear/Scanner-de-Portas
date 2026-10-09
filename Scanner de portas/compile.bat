@echo off
REM ============================================================
REM compile.bat - Compila o scanner com MinGW (gcc) no Windows
REM Requer: gcc (MinGW-w64) no PATH.
REM Gera: scanner_simple.exe (standalone) e scanner.exe (completo)
REM ============================================================
setlocal
where gcc >nul 2>&1
if errorlevel 1 (
    echo [ERRO] gcc nao encontrado no PATH. Instale o MinGW-w64.
    exit /b 1
)

echo [*] Compilando versao standalone (scanner_simple.exe)...
gcc -O2 -Wall scanner_simple.c -o scanner_simple.exe -lws2_32
if errorlevel 1 goto :err

echo [*] Compilando versao completa (scanner.exe)...
gcc -O2 -Wall -I src src\main.c src\scanner.c -o scanner.exe -lws2_32
if errorlevel 1 goto :err

echo [OK] Build concluido: scanner_simple.exe e scanner.exe
exit /b 0

:err
echo [ERRO] Falha na compilacao.
exit /b 1
