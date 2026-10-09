@echo off
REM ============================================================
REM compile_vs.bat - Compila o scanner com o Visual Studio (MSVC)
REM Requer: "Developer Command Prompt for VS" OU chamar vcvars64.bat antes.
REM Gera: scanner_simple.exe (standalone) e scanner.exe (completo)
REM ============================================================
setlocal
where cl >nul 2>&1
if errorlevel 1 (
    echo [ERRO] cl.exe nao encontrado. Abra o "x64 Native Tools Command Prompt for VS"
    echo         ou execute vcvars64.bat antes de rodar este script.
    exit /b 1
)

echo [*] Compilando versao standalone (scanner_simple.exe)...
cl /nologo /W3 scanner_simple.c ws2_32.lib /Fe:scanner_simple.exe
if errorlevel 1 goto :err

echo [*] Compilando versao completa (scanner.exe)...
cl /nologo /W3 /I src src\main.c src\scanner.c ws2_32.lib /Fe:scanner.exe
if errorlevel 1 goto :err

echo [OK] Build concluido: scanner_simple.exe e scanner.exe
del *.obj >nul 2>&1
exit /b 0

:err
echo [ERRO] Falha na compilacao.
exit /b 1
