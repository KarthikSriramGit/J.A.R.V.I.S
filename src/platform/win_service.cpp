#ifdef _WIN32

#include "platform/win_service.h"
#include "core/logger.h"
#include <windows.h>
#include <string>

namespace jarvis {
namespace platform {

static SERVICE_STATUS status;
static SERVICE_STATUS_HANDLE status_handle = nullptr;
static HANDLE stop_event = nullptr;

static void report_status(DWORD state) {
    status.dwCurrentState = state;
    status.dwWin32ExitCode = 0;
    SetServiceStatus(status_handle, &status);
}

static VOID WINAPI service_ctrl(DWORD ctrl) {
    switch (ctrl) {
        case SERVICE_CONTROL_STOP:
            report_status(SERVICE_STOPPED);
            SetEvent(stop_event);
            break;
        default:
            break;
    }
}

bool is_running_as_service() {
    return GetEnvironmentVariableA("JARVIS_SERVICE", nullptr, 0) > 0;
}

int service_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    stop_event = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    status_handle = RegisterServiceCtrlHandlerA("JARVIS", service_ctrl);
    if (!status_handle) return 1;

    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwServiceSpecificExitCode = 0;
    report_status(SERVICE_RUNNING);

    jarvis::core::JARVIS_LOG_INFO("J.A.R.V.I.S. service running.");
    WaitForSingleObject(stop_event, INFINITE);
    report_status(SERVICE_STOPPED);
    CloseHandle(stop_event);
    return 0;
}

}  // namespace platform
}  // namespace jarvis

#endif
