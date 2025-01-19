#include "pch.h"

#include <windows.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>
#include <atomic>

// Função para registrar logs em um arquivo
void Log(const std::string& message) {
    std::ofstream logFile("loader_log.txt", std::ios_base::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.close();
    }
}

// Variável global para evitar carregamento duplicado
std::atomic<bool> dllLoaded(false);

// Função que será exportada pela DLL
extern "C" __declspec(dllexport) void LoadBackendDLL() {
    if (!dllLoaded.load()) {
        Log("Tentando carregar a DLL.");

        HMODULE hModule = LoadLibrary(L"Dumper-7.dll"); // Usando L prefix
        if (hModule != NULL) {
            Log("DLL carregada com sucesso.");
            dllLoaded.store(true); // Atualiza o estado para indicar que a DLL foi carregada
        }
        else {
            Log("Falha ao carregar a DLL.");
        }
    }
    else {
        Log("DLL já foi carregada anteriormente. Evitando carregamento duplicado.");
    }
}

// Função para monitorar a tecla de atalho "ctrl+o"
void MonitorShortcut() {
    while (true) {
        if (GetAsyncKeyState('O') & 0x8000 && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            Log("Tecla de atalho 'ctrl+o' pressionada.");
            LoadBackendDLL();
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Evita uso excessivo de CPU
    }
}

// Função chamada quando a DLL é carregada
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // Inicia a thread de monitoramento da tecla de atalho ao anexar a DLL ao processo
        std::thread(MonitorShortcut).detach();
        break;
    case DLL_PROCESS_DETACH:
        Log("DLL descarregada do processo.");
        break;
    }
    return TRUE;
}
