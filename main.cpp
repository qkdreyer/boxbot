/* 
 * 
 * File:   main.cpp
 * Author: Quentin
 *
 * Created on 5 novembre 2011, 18:43
 */

#define _WIN32_WINNT 0x0501
#define VK_Z 0x5a
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <pthread.h>

HWND handle;
POINT cursor;
int lootTime = 800;
int sellTime = 800;

void* CheckFocus(void* data);
void* LootAndSell(void* data);
void* Quit(void* data);
void SendClick(int n, int s, int x, int y);
void SendKey(int n, int s, int a, int b);

void Set_Cursor_Pos(int x, int y) {
    INPUT Input = {0};
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    Input.mi.dx = x * (65535.0f / GetSystemMetrics(SM_CXSCREEN) - 1);
    Input.mi.dy = y * (65535.0f / GetSystemMetrics(SM_CYSCREEN) - 1);
    SendInput(1, &Input, sizeof (INPUT));
}

void* CheckFocus(void* data) {
    while ((GetForegroundWindow() == handle) == (BOOL) data) {
        Sleep(10);
    }
    return NULL;
}

void* LootAndSell(void* data) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while (1) {
        SendKey(1, 1000, VK_ESCAPE, 0x10001); // Escape
        SendClick(1, 10, (int) cursor.x, (int) cursor.y); // System Menu Fix
        SendKey(lootTime, 10, VK_Z, 0x2c0001); // Z
        SendKey(1, 1000, VK_SPACE, 0x390001); // Space
        SendClick(10, 10, (int) cursor.x, (int) cursor.y); // Use
        SendClick(sellTime, 10, (int) cursor.x, (int) cursor.y + 100); // Sell
    }
}

void* Quit(void* data) {
    std::cout << "quit" << std::endl;
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        std::cout << "msg " << msg.message;
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_F12) {
            return 0;
        }
    }
}

void SendClick(int n, int s, int x, int y) {
    SetCursorPos(x, y);
    for (int i = 0; i < n; i++) {
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        Sleep(s);
    }
}

void SendKey(int n, int s, int a, int b) {
    for (int i = 0; i < n; i++) {
        PostMessage(handle, WM_KEYDOWN, a, b);
        PostMessage(handle, WM_KEYUP, a, b);
        Sleep(s);
    }
}

int main(int argc, char* argv[]) {

    if (argc == 3) {
        sscanf(argv[1], "%d", &lootTime);
        sscanf(argv[2], "%d", &sellTime);
        lootTime = lootTime / 10;
        sellTime = sellTime / 10;
    }

    ShowWindow(GetConsoleWindow(), SW_HIDE);
    handle = FindWindow(NULL, "MapleStory");
    if (handle != NULL) {
        SetForegroundWindow(handle);
        SendKey(1, 3000, 0x20, 0x390001); // Space
        GetCursorPos(&cursor);

        pthread_t CheckFocusTh, LootAndSellTh, QuitTh;
        pthread_create(&QuitTh, NULL, Quit, NULL);
        while (1) {
            pthread_create(&CheckFocusTh, NULL, CheckFocus, (void*) TRUE);
            pthread_create(&LootAndSellTh, NULL, LootAndSell, NULL);
            pthread_join(CheckFocusTh, NULL);
            pthread_cancel(LootAndSellTh);
            pthread_create(&CheckFocusTh, NULL, CheckFocus, (void*) FALSE);
            pthread_join(CheckFocusTh, NULL);
        }
    } else {
        return 0;
    }
}
