# Scanner de Portas TCP (Windows) 🔍

Scanner TCP simplificado para Windows escrito em C usando Winsock. Implementa varredura de conexão TCP com suporte a multithreading e timeouts configuráveis. Alternativa leve ao Nmap para varreduras básicas.

## 📁 Estrutura do Projeto

### Versões Disponíveis
- **Versão Completa** (multithreaded):
  - `src/main.c` - Interface CLI e inicialização
  - `src/scanner.c` - Scanner com pool de threads Win32
  - `src/scanner.h` - Definições e protótipos
- **Versão Simplificada** (single-thread):
  - `scanner_simple.c` - Versão standalone sem dependências extras
- **Teste Básico**:
  - `test.c` - Teste mínimo de Winsock

### Scripts de Compilação
- `compile.bat` - Script automático para MinGW/MSYS2
- `compile_vs.bat` - Script para Visual Studio Build Tools
- `compile.sh` - Script para ambiente MSYS2 bash

## 🔧 Compilação

### Opção 1: MSYS2 UCRT64 (Recomendada)

1. **Instalar MSYS2**:
   - Baixe em: https://www.msys2.org/
   - Execute o instalador e instale em `C:\msys64`

2. **Configurar ambiente**:
   ```bash
   # No terminal MSYS2 UCRT64
   pacman -Syu
   pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
   ```

3. **Compilar**:
   ```bash
   # Abra "MSYS2 UCRT64" (não o PowerShell!)
   cd "/c/Users/Henry/OneDrive/Área de Trabalho/Scanner de portas"
   
   # Versão simplificada (recomendada)
   gcc -O2 scanner_simple.c -lws2_32 -o scanner_simple.exe
   
   # Versão completa (com threads)
   gcc -O2 src/main.c src/scanner.c -lws2_32 -o scanner.exe
   ```

### Opção 2: Visual Studio

1. **Abrir "x64 Native Tools Command Prompt for VS"**
2. **Compilar**:
   ```cmd
   # Versão simplificada
   cl /nologo scanner_simple.c ws2_32.lib /Fe:scanner_simple.exe
   
   # Versão completa
   cl /nologo src\main.c src\scanner.c ws2_32.lib /Fe:scanner.exe
   ```

### Opção 3: Scripts Automáticos

```powershell
# Tenta encontrar e usar compilador automaticamente
.\compile_vs.bat
```

## 🚀 Uso

### Sintaxe
```bash
# Versão simplificada
./scanner_simple.exe <host> <start>-<end> [timeout_ms]

# Versão completa
./scanner.exe <host> <start>-<end> [timeout_ms] [threads]
```

### Exemplos
```bash
# Escanear portas comuns do localhost
./scanner_simple.exe 127.0.0.1 1-1024 200

# Escanear porta específica
./scanner_simple.exe google.com 80-80 100

# Versão com threads (se compilada)
./scanner.exe 192.168.1.1 1-65535 150 50
```

### Saída Esperada
```
Scanning 127.0.0.1 ports 1-1024 (timeout 200 ms)
22/tcp open
80/tcp open
443/tcp open

Scan complete. Found 3 open ports.
```

## ⚙️ Funcionalidades

- ✅ **Varredura TCP Connect** - Conexões TCP completas
- ✅ **Timeouts Configuráveis** - Controle de tempo limite por porta
- ✅ **Suporte IPv4/IPv6** - Resolução automática via `getaddrinfo`
- ✅ **Conexões Não-bloqueantes** - Usa `select()` para timeout eficiente
- ✅ **Multithreading** - Pool de workers Win32 (versão completa)
- ✅ **Saída Limpa** - Mostra apenas portas abertas por padrão

## 📋 Limitações e Notas

### Requisitos
- **Windows** (usa Winsock2)
- **Privilégios normais** (não requer admin)
- **Compilador C** (MinGW ou Visual Studio)

### Limitações Técnicas
- **TCP Connect Scan apenas** - Não implementa SYN scan
- **Sem UDP** - Apenas varredura TCP
- **Rate limiting básico** - Controlado por número de threads
- **Resolução simples** - Usa primeiro endereço retornado

### Considerações de Rede
- **Firewalls** podem causar timeouts (mostrado como filtered)
- **Rate limiting** em roteadores pode afetar velocidade
- **IDS/IPS** podem detectar varreduras intensas

## 🔒 Aspectos Legais e Éticos

⚠️ **IMPORTANTE**: 
- Use apenas em hosts que você **possui** ou tem **permissão explícita**
- Varreduras não autorizadas podem violar leis locais
- Teste sempre em ambiente controlado (localhost) primeiro

## 🛠️ Solução de Problemas

### Erro de Compilação
```bash
# Se gcc não for encontrado no PowerShell:
# 1. Use o terminal "MSYS2 UCRT64" diretamente
# 2. Ou adicione C:\msys64\ucrt64\bin ao PATH do Windows
```

### Erro de Execução
```bash
# Testar Winsock básico
./test.exe

# Verificar sintaxe
./scanner_simple.exe
```

### Performance
```bash
# Para varreduras grandes, ajuste threads e timeout:
./scanner.exe target 1-65535 100 100  # 100 threads, timeout 100ms
```

## 🚀 Melhorias Futuras

- [ ] Suporte a varredura SYN (raw sockets)
- [ ] Detecção de serviços (banner grabbing)
- [ ] Saída em formatos JSON/XML
- [ ] Suporte a listas de hosts
- [ ] Rate limiting inteligente
- [ ] Retry automático para timeouts
- [ ] IPv6 melhorado
- [ ] Logging detalhado

## 📞 Suporte

Para problemas de compilação:
1. Verifique se o MSYS2 está instalado corretamente
2. Use o terminal correto ("MSYS2 UCRT64")
3. Teste com `gcc --version` primeiro
4. Use a versão simplificada como fallback

---

**Criado com**: C, Winsock2, Win32 Threads  
**Compatibilidade**: Windows 7+ (x64)  
**Licença**: Uso educacional e teste em ambiente próprio
