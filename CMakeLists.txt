cmake_minimum_required(VERSION 3.24)
project(JARVIS VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# vcpkg
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
endif()

# Find packages (Phase 1+2 deps)
find_package(nlohmann_json REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(cpp-httplib REQUIRED)
find_package(OpenCV REQUIRED)
find_package(dlib REQUIRED)

# miniaudio is header-only; we need one .c file with MINIAUDIO_IMPLEMENTATION
# vcpkg installs miniaudio.h; we add our impl
add_library(miniaudio_impl STATIC src/audio/miniaudio_impl.c)
target_include_directories(miniaudio_impl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(miniaudio_impl PRIVATE MINIAUDIO_IMPLEMENTATION)

add_executable(jarvis
  src/main.cpp
  src/core/config.cpp
  src/core/logger.cpp
  src/core/orchestrator.cpp
  src/core/event_bus.cpp
  src/vision/camera.cpp
  src/vision/face_recognition.cpp
  src/audio/player.cpp
  src/audio/audio_io.cpp
  src/audio/stt.cpp
  src/audio/tts.cpp
  src/brain/personality.cpp
  src/brain/llm_local.cpp
  src/brain/llm_cloud.cpp
  src/brain/router.cpp
  src/agent/tool_registry.cpp
  src/agent/file_ops.cpp
  src/agent/shell_exec.cpp
  src/agent/code_search.cpp
  src/agent/code_agent.cpp
  src/platform/startup.cpp
)

if(WIN32)
  target_sources(jarvis PRIVATE
    src/platform/system_tray.cpp
    src/platform/win_service.cpp
  )
endif()

target_include_directories(jarvis PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(jarvis PRIVATE
  ${OpenCV_LIBS}
  dlib::dlib
  nlohmann_json::nlohmann_json
  yaml-cpp
  cpp-httplib::cpp-httplib
  miniaudio_impl
)

# Face enrollment utility
add_executable(jarvis_enroll
  src/vision/enroll_face.cpp
  src/core/config.cpp
  src/core/logger.cpp
  src/vision/camera.cpp
  src/vision/face_recognition.cpp
)
target_include_directories(jarvis_enroll PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(jarvis_enroll PRIVATE
  ${OpenCV_LIBS}
  dlib::dlib
  nlohmann_json::nlohmann_json
  yaml-cpp
)

# Copy assets to build dir
add_custom_command(TARGET jarvis POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:jarvis>/assets
)
add_custom_command(TARGET jarvis POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/config
    $<TARGET_FILE_DIR:jarvis>/config
)

# Install
install(TARGETS jarvis jarvis_enroll RUNTIME DESTINATION bin)
install(DIRECTORY assets config DESTINATION share/jarvis)
