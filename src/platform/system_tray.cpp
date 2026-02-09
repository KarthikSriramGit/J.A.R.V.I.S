#ifdef _WIN32

#include "platform/system_tray.h"
#include "core/logger.h"
#include <windows.h>
#include <shellapi.h>

namespace jarvis {
namespace platform {

namespace {
SystemTray* g_tray = nullptr;
const UINT WM_TRAYICON = WM_USER + 1;
const UINT IDM_QUIT = 1001;
const UINT IDM_QUIET = 1002;
const UINT IDM_ACTIVE = 1003;
}

static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_TRAYICON) {
        if (lparam == WM_RBUTTONUP && g_tray && g_tray->callback_) {
            POINT pt;
            GetCursorPos(&pt);
            HMENU menu = CreatePopupMenu();
            AppendMenu(menu, MF_STRING, IDM_ACTIVE, L"Active");
            AppendMenu(menu, MF_STRING, IDM_QUIET, L"Quiet mode");
            AppendMenu(menu, MF_SEPARATOR, 0, nullptr);
            AppendMenu(menu, MF_STRING, IDM_QUIT, L"Exit");
            SetForegroundWindow(hwnd);
            int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, nullptr);
            DestroyMenu(menu);
            if (cmd && g_tray && g_tray->callback_) g_tray->callback_(cmd);
        }
        return 0;
    }
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

SystemTray::SystemTray() {
    g_tray = this;
}

SystemTray::~SystemTray() {
    hide();
    g_tray = nullptr;
}

bool SystemTray::show() {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"JARVISTray";
    RegisterClassEx(&wc);

    hwnd_ = CreateWindowEx(0, L"JARVISTray", L"", 0, 0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);
    if (!hwnd_) return false;

    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = (HWND)hwnd_;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"J.A.R.V.I.S.");
    Shell_NotifyIcon(NIM_ADD, &nid);
    icon_ = (void*)nid.hIcon;
    return true;
}

void SystemTray::hide() {
    if (hwnd_) {
        NOTIFYICONDATA nid = {};
        nid.cbSize = sizeof(nid);
        nid.hWnd = (HWND)hwnd_;
        nid.uID = 1;
        Shell_NotifyIcon(NIM_DELETE, &nid);
        DestroyWindow((HWND)hwnd_);
        hwnd_ = nullptr;
    }
}

void SystemTray::run_message_loop() {
    running_ = true;
    MSG msg;
    while (running_ && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

}  // namespace platform
}  // namespace jarvis

#endif
