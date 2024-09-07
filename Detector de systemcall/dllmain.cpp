#include <windows.h>
#include <iostream>
#include "MinHook.h"

// Tipos de função para as chamadas de sistema
typedef HANDLE(WINAPI* pCreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
typedef BOOL(WINAPI* pReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef BOOL(WINAPI* pWriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef BOOL(WINAPI* pCloseHandle)(HANDLE);
typedef BOOL(WINAPI* pGetFileInformationByHandle)(HANDLE, LPBY_HANDLE_FILE_INFORMATION);

pCreateFileW OriginalCreateFileW = nullptr;
pReadFile OriginalReadFile = nullptr;
pWriteFile OriginalWriteFile = nullptr;
pCloseHandle OriginalCloseHandle = nullptr;
pGetFileInformationByHandle OriginalGetFileInformationByHandle = nullptr;

HANDLE WINAPI HookedCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    std::wcout << L"System call detected: CreateFileW called for: " << lpFileName << std::endl;
    return OriginalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

BOOL WINAPI HookedReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) {
    std::cout << "System call detected: ReadFile called." << std::endl;
    return OriginalReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

BOOL WINAPI HookedWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) {
    std::cout << "System call detected: WriteFile called." << std::endl;
    return OriginalWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

BOOL WINAPI HookedCloseHandle(HANDLE hObject) {
    std::cout << "System call detected: CloseHandle called." << std::endl;
    return OriginalCloseHandle(hObject);
}

BOOL WINAPI HookedGetFileInformationByHandle(HANDLE hFile, LPBY_HANDLE_FILE_INFORMATION lpFileInformation) {
    std::cout << "System call detected: GetFileInformationByHandle called." << std::endl;
    return OriginalGetFileInformationByHandle(hFile, lpFileInformation);
}

DWORD WINAPI Initialize(LPVOID lpParameter) {
    // Inicializa MinHook
    if (MH_Initialize() != MH_OK) {
        return 1;
    }

    // Cria hooks para as funções
    if (MH_CreateHookApi(L"kernel32.dll", "CreateFileW", &HookedCreateFileW, reinterpret_cast<LPVOID*>(&OriginalCreateFileW)) != MH_OK ||
        MH_CreateHookApi(L"kernel32.dll", "ReadFile", &HookedReadFile, reinterpret_cast<LPVOID*>(&OriginalReadFile)) != MH_OK ||
        MH_CreateHookApi(L"kernel32.dll", "WriteFile", &HookedWriteFile, reinterpret_cast<LPVOID*>(&OriginalWriteFile)) != MH_OK ||
        MH_CreateHookApi(L"kernel32.dll", "CloseHandle", &HookedCloseHandle, reinterpret_cast<LPVOID*>(&OriginalCloseHandle)) != MH_OK ||
        MH_CreateHookApi(L"kernel32.dll", "GetFileInformationByHandle", &HookedGetFileInformationByHandle, reinterpret_cast<LPVOID*>(&OriginalGetFileInformationByHandle)) != MH_OK) {
        return 1;
    }

    // Ativa os hooks
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        return 1;
    }

    // Mantém o console aberto e continua monitorando
    std::cout << "Monitoring system calls. Press Ctrl+C to exit." << std::endl;
    while (true) {
        Sleep(1000); // Pausa para evitar uso excessivo da CPU
    }

    // Desativa os hooks e finaliza MinHook (não alcançado neste exemplo)
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    return 0;
}

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        // Abre um console para exibir mensagens
        AllocConsole();
        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

        // Cria uma thread para inicializar o hook
        CreateThread(NULL, 0, Initialize, NULL, 0, NULL);
    }
    return TRUE;
}
