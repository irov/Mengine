@echo off

@call vcvarsall_msvc15

@pushd ..
@call make_solution "%CD%\..\CMake\Win32_SDL" solution_msvc15_sdl_debug "Visual Studio 15 2017" Debug build_msvc15_sdl_debug
@popd

@echo on

@pause
