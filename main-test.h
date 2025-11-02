#ifndef MASTER_H
#define MASTER_H

#include <Windows.h>
#include <TlHelp32.h>
#include <cstring>
#include <cstdio>
using namespace std;

HWND hWnd = GetConsoleWindow();
DWORD main_pid = 0;
HWND hBoardWindow;
HHOOK kbdHook;
HHOOK mseHook;

DWORD GetPidFromHwnd(HWND hwnd) {
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid;
}

void SuspendProcess(DWORD process_id) {
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);
    Thread32First(hThreadSnapshot, &threadEntry);
    do {
        if (threadEntry.th32OwnerProcessID == process_id) {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE,threadEntry.th32ThreadID);
            SuspendThread(hThread);
            CloseHandle(hThread);
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));
    CloseHandle(hThreadSnapshot);
}

void ResumeProcess(DWORD process_id) {
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);
    Thread32First(hThreadSnapshot, &threadEntry);
    do {
        if (threadEntry.th32OwnerProcessID == process_id) {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE,threadEntry.th32ThreadID);
            ResumeThread(hThread);
            CloseHandle(hThread);
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));
    CloseHandle(hThreadSnapshot);
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(kbdHook, nCode, wParam, lParam);
}
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(mseHook, nCode, wParam, lParam);
}

DWORD WINAPI KeyHookLLThreadProc(LPVOID lpParameter) {
    kbdHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);
    if (kbdHook == NULL) {
        return 1;
    }
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (kbdHook) {
        UnhookWindowsHookEx(kbdHook);
        kbdHook = NULL;
    }
    return 0;
}

DWORD WINAPI MouseHookLLThreadProc(LPVOID lpParameter) {
    mseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, GetModuleHandle(NULL), 0);
    if (mseHook == NULL) {
        return 1;
    }
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (mseHook) {
        UnhookWindowsHookEx(mseHook);
        mseHook = NULL;
    }
    return 0;
}

void WindowHookProc() {
    if (!IsWindow(hBoardWindow)) {
        hBoardWindow = GetForegroundWindow();
        if (hBoardWindow == NULL) {
            ResumeProcess(main_pid);
            return;
        }
        main_pid = GetPidFromHwnd(hBoardWindow);
    }
    if (!IsIconic(hBoardWindow)) {
        ShowWindow(hBoardWindow, SW_MINIMIZE);
        SuspendProcess(main_pid);
    }
    else {
        ResumeProcess(main_pid);
        ShowWindow(hBoardWindow, SW_RESTORE);
        hBoardWindow = NULL;
    }
    return;
}

DWORD WINAPI WindowHookThreadProc(LPVOID lpParameter) {
    RegisterHotKey(NULL, 1, NULL, VK_F1);
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            WindowHookProc();
        }
    }
}

#endif // MASTER_H
