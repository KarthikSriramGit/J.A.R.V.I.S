#pragma once

#ifdef _WIN32

#include <string>

namespace jarvis {
namespace platform {

bool is_running_as_service();
int service_main(int argc, char* argv[]);

}  // namespace platform
}  // namespace jarvis

#endif
