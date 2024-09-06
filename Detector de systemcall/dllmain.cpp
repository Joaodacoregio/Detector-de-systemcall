#include "pch.h"
#include <detours.h>
#include <windows.h>


typedef int (WINAPI* MessageBoxA_t)(HWND, LPCSTR, LPCSTR, UINT);
MessageBoxA_t OriginalMessageBoxA = nullptr;


int WINAPI HookedMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    // Alterar o texto da mensagem
    return OriginalMessageBoxA(hWnd, "Mensagem interceptada!", "Hooked", uType);
}

void HookFunction() {
    // Obt�m o endere�o da fun��o original MessageBoxA
    OriginalMessageBoxA = (MessageBoxA_t)GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxA");

    // Inicia uma transa��o de detour
    DetourTransactionBegin();

    DetourUpdateThread(GetCurrentThread());

    // Realiza o hook da fun��o MessageBoxA
    DetourAttach(&(PVOID&)OriginalMessageBoxA, HookedMessageBoxA);

    // Conclui a transa��o de detour
    DetourTransactionCommit();
 
}

void UnhookFunction() {
    // Inicia uma transa��o de detour
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // Desfaz o hook da fun��o MessageBoxA
    DetourDetach(&(PVOID&)OriginalMessageBoxA, HookedMessageBoxA);

    // Conclui a transa��o de detour
    DetourTransactionCommit();
}
 


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        MessageBox(NULL, L"DLL carregada com sucesso!", L"Informa��o", MB_OK);
        HookFunction();
         
        break;
    case DLL_THREAD_ATTACH:
        MessageBox(NULL, L"DLL finalizada", L"Informa��o", MB_OK);
        UnhookFunction();
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
