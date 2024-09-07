#include <windows.h>
#include "MinHook.h"  // Inclui o MinHook
 
typedef int (WINAPI* MessageBoxA_t)(HWND, LPCSTR, LPCSTR, UINT);
MessageBoxA_t OriginalMessageBoxA = NULL;

// Função hookada
int WINAPI HookedMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    return OriginalMessageBoxA(hWnd, "Hooked Message!", "Intercepted", uType);
}

void InstallHook() {
 
    // Inicializa o MinHook
    MH_Initialize();

    // Cria o hook para MessageBoxA
    MH_CreateHook(&MessageBoxA, &HookedMessageBoxA, reinterpret_cast<LPVOID*>(&OriginalMessageBoxA));

    // Ativa o hook
    MH_EnableHook(&MessageBoxA);
}

void RemoveHook() {
    // Desativa o hook
    MH_DisableHook(&MessageBoxA);

    // Finaliza o MinHook
    MH_Uninitialize();
}

// Função principal da DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        MessageBoxA(NULL, "DLL INJETADO", "Hook", MB_OK);
        InstallHook();  // Instala o hook quando a DLL é carregada
        MessageBoxA(NULL, "Se funcionou essa msg não deve aparecer!", "Hook", MB_OK);
        break;
    case DLL_PROCESS_DETACH:
        RemoveHook();   // Remove o hook quando a DLL é descarregada
        break;
    }
    return TRUE;
}
