CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(sentry)

SET(SENTRY_SOURCE_DIR ${THIRDPARTY_DIR}/sentry)

# OUR SOURCES
set(sentry_sources
${SENTRY_SOURCE_DIR}/src/api.cpp
${SENTRY_SOURCE_DIR}/src/attachment.hpp
${SENTRY_SOURCE_DIR}/src/cleanup.cpp
${SENTRY_SOURCE_DIR}/src/cleanup.hpp
${SENTRY_SOURCE_DIR}/src/dsn.cpp
${SENTRY_SOURCE_DIR}/src/dsn.hpp
${SENTRY_SOURCE_DIR}/src/internal.hpp
${SENTRY_SOURCE_DIR}/src/io.cpp
${SENTRY_SOURCE_DIR}/src/io.hpp
${SENTRY_SOURCE_DIR}/src/json.hpp
${SENTRY_SOURCE_DIR}/src/modulefinder.hpp
${SENTRY_SOURCE_DIR}/src/options.cpp
${SENTRY_SOURCE_DIR}/src/options.hpp
${SENTRY_SOURCE_DIR}/src/path.cpp
${SENTRY_SOURCE_DIR}/src/path.hpp
${SENTRY_SOURCE_DIR}/src/scope.cpp
${SENTRY_SOURCE_DIR}/src/scope.hpp
${SENTRY_SOURCE_DIR}/src/symbolize.hpp
${SENTRY_SOURCE_DIR}/src/unwind.cpp
${SENTRY_SOURCE_DIR}/src/unwind.hpp
${SENTRY_SOURCE_DIR}/src/url.cpp
${SENTRY_SOURCE_DIR}/src/url.hpp
${SENTRY_SOURCE_DIR}/src/uuid.cpp
${SENTRY_SOURCE_DIR}/src/uuid.hpp
${SENTRY_SOURCE_DIR}/src/value.cpp
${SENTRY_SOURCE_DIR}/src/value.hpp
${SENTRY_SOURCE_DIR}/src/worker.cpp
${SENTRY_SOURCE_DIR}/src/worker.hpp
${SENTRY_SOURCE_DIR}/src/backends/base_backend.hpp
${SENTRY_SOURCE_DIR}/src/backends/crashpad_backend.cpp
${SENTRY_SOURCE_DIR}/src/backends/crashpad_backend.hpp
${SENTRY_SOURCE_DIR}/src/modulefinders/windows_modulefinder.cpp
${SENTRY_SOURCE_DIR}/src/symbolizers/windows_symbolizer.cpp
${SENTRY_SOURCE_DIR}/src/transports/base_transport.cpp
${SENTRY_SOURCE_DIR}/src/transports/base_transport.hpp
${SENTRY_SOURCE_DIR}/src/transports/envelopes.cpp
${SENTRY_SOURCE_DIR}/src/transports/envelopes.hpp
${SENTRY_SOURCE_DIR}/src/transports/function_transport.cpp
${SENTRY_SOURCE_DIR}/src/transports/function_transport.hpp
${SENTRY_SOURCE_DIR}/src/transports/winhttp_transport.cpp
${SENTRY_SOURCE_DIR}/src/transports/winhttp_transport.hpp
${SENTRY_SOURCE_DIR}/src/unwinders/windows_unwinder.cpp
${SENTRY_SOURCE_DIR}/src/vendor/catch.hpp
${SENTRY_SOURCE_DIR}/src/vendor/mpack.c
${SENTRY_SOURCE_DIR}/src/vendor/mpack.h
)

SET(CRASHPAD_SOURCE_DIR ${SENTRY_SOURCE_DIR}/crashpad/build/crashpad)

set(chashpad_sources
${CRASHPAD_SOURCE_DIR}/client/annotation.cc
${CRASHPAD_SOURCE_DIR}/client/annotation_list.cc
${CRASHPAD_SOURCE_DIR}/client/crash_report_database.cc
${CRASHPAD_SOURCE_DIR}/client/crash_report_database_win.cc
${CRASHPAD_SOURCE_DIR}/client/crashpad_client_win.cc
${CRASHPAD_SOURCE_DIR}/client/crashpad_info.cc
${CRASHPAD_SOURCE_DIR}/client/prune_crash_reports.cc
${CRASHPAD_SOURCE_DIR}/client/settings.cc
${CRASHPAD_SOURCE_DIR}/util/win/scoped_handle.cc
${CRASHPAD_SOURCE_DIR}/util/win/command_line.cc
${CRASHPAD_SOURCE_DIR}/util/win/critical_section_with_debug_info.cc
${CRASHPAD_SOURCE_DIR}/util/win/get_function.cc
${CRASHPAD_SOURCE_DIR}/util/win/handle.cc
${CRASHPAD_SOURCE_DIR}/util/win/initial_client_data.cc
${CRASHPAD_SOURCE_DIR}/util/win/nt_internals.cc
${CRASHPAD_SOURCE_DIR}/util/win/ntstatus_logging.cc
${CRASHPAD_SOURCE_DIR}/util/win/process_info.cc
${CRASHPAD_SOURCE_DIR}/util/win/registration_protocol_win.cc
${CRASHPAD_SOURCE_DIR}/util/win/safe_terminate_process_broken.cc
${CRASHPAD_SOURCE_DIR}/util/win/scoped_process_suspend.cc
${CRASHPAD_SOURCE_DIR}/util/file/file_io.cc
${CRASHPAD_SOURCE_DIR}/util/file/file_io_win.cc
${CRASHPAD_SOURCE_DIR}/util/file/file_reader.cc
${CRASHPAD_SOURCE_DIR}/util/file/file_writer.cc
${CRASHPAD_SOURCE_DIR}/util/file/scoped_remove_file.cc
${CRASHPAD_SOURCE_DIR}/util/file/directory_reader_win.cc
${CRASHPAD_SOURCE_DIR}/util/file/filesystem_win.cc
${CRASHPAD_SOURCE_DIR}/util/misc/metrics.cc
${CRASHPAD_SOURCE_DIR}/util/misc/uuid.cc
${CRASHPAD_SOURCE_DIR}/util/misc/initialization_state_dcheck.cc
${CRASHPAD_SOURCE_DIR}/util/misc/capture_context_broken.cc
${CRASHPAD_SOURCE_DIR}/util/misc/random_string.cc
${CRASHPAD_SOURCE_DIR}/util/misc/time_win.cc
${CRASHPAD_SOURCE_DIR}/util/stdlib/aligned_allocator.cc
${CRASHPAD_SOURCE_DIR}/util/stdlib/string_number_conversion.cc
${CRASHPAD_SOURCE_DIR}/util/string/split_string.cc
)

SET(MINI_CHROMIUM_SOURCE_DIR ${SENTRY_SOURCE_DIR}/crashpad/build/crashpad/third_party/mini_chromium/mini_chromium)

set(mini_chromium_sources
${MINI_CHROMIUM_SOURCE_DIR}/base/files/file_path.cc
${MINI_CHROMIUM_SOURCE_DIR}/base/strings/utf_string_conversions.cc
${MINI_CHROMIUM_SOURCE_DIR}/base/strings/stringprintf.cc
${MINI_CHROMIUM_SOURCE_DIR}/base/strings/string_util_win.cc
${MINI_CHROMIUM_SOURCE_DIR}/base/strings/utf_string_conversion_utils.cc
${MINI_CHROMIUM_SOURCE_DIR}/base/synchronization/lock_impl_win.cc
${MINI_CHROMIUM_SOURCE_DIR}/base/synchronization/lock.cc
${MINI_CHROMIUM_SOURCE_DIR}/base/process/memory.cc
${MINI_CHROMIUM_SOURCE_DIR}/base/logging.cc
${MINI_CHROMIUM_SOURCE_DIR}/base/rand_util.cc
${MINI_CHROMIUM_SOURCE_DIR}/base/third_party/icu/icu_utf.cc
)
              
include_directories(${SENTRY_SOURCE_DIR}/include)
include_directories(${SENTRY_SOURCE_DIR}/crashpad/build/crashpad)
include_directories(${SENTRY_SOURCE_DIR}/crashpad/build/crashpad/third_party/mini_chromium/mini_chromium)
include_directories(${SENTRY_SOURCE_DIR}/crashpad/build/crashpad/compat/win)
include_directories(${SENTRY_SOURCE_DIR}/crashpad/build/crashpad/compat/non_mac)
include_directories(${SENTRY_SOURCE_DIR}/crashpad/build/crashpad/compat/non_elf)

add_definitions(-DSENTRY_WITH_WINHTTP_TRANSPORT)
add_definitions(-DSENTRY_WITH_WINDOWS_MODULEFINDER)
add_definitions(-DSENTRY_WITH_WINDOWS_SYMBOLIZER)
add_definitions(-DSENTRY_WITH_WINDOWS_UNWINDER)
add_definitions(-DSENTRY_WITH_CRASHPAD_BACKEND)
add_definitions(-DNOMINMAX)
add_definitions(-DWIN32_LEAN_AND_MEAN)
add_definitions(-DSENTRY_BUILD_STATIC)

add_library(${PROJECT_NAME} STATIC ${sentry_sources} ${chashpad_sources} ${mini_chromium_sources})