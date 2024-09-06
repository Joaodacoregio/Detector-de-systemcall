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
    // Obtém o endereço da função original MessageBoxA
    OriginalMessageBoxA = (MessageBoxA_t)GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxA");

    // Inicia uma transação de detour
    DetourTransactionBegin();

    DetourUpdateThread(GetCurrentThread());

    // Realiza o hook da função MessageBoxA
    DetourAttach(&(PVOID&)OriginalMessageBoxA, HookedMessageBoxA);

    // Conclui a transação de detour
    DetourTransactionCommit();
 
}

void UnhookFunction() {
    // Inicia uma transação de detour
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // Desfaz o hook da função MessageBoxA
    DetourDetach(&(PVOID&)OriginalMessageBoxA, HookedMessageBoxA);

    // Conclui a transação de detour
    DetourTransactionCommit();
}
 


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        MessageBox(NULL, L"DLL carregada com sucesso!", L"Informação", MB_OK);
        HookFunction();
         
        break;
    case DLL_THREAD_ATTACH:
        MessageBox(NULL, L"DLL finalizada", L"Informação", MB_OK);
        UnhookFunction();
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
