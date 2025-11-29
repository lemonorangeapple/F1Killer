#include "main.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    MessageBox(NULL, "By June_qwq(lemonorangeapple)", "F1Killer", MB_OK);
    CreateThread(NULL, 0, KeyHookLLThreadProc, NULL, 0, NULL);
    CreateThread(NULL, 0, MouseHookLLThreadProc, NULL, 0, NULL);
    CreateThread(NULL, 0, WindowHookThreadProc, NULL, 0, NULL);
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {}
}