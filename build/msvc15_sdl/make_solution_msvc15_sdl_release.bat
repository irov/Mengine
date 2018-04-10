@echo off

@pushd ..
@call vcvarsall_msvc15.bat
@popd

@pushd ..
@call make_solution.bat "%CD%\..\CMake\Win32_SDL" solution_msvc15_sdl_release "Visual Studio 15 2017" Release build_msvc15_sdl_release
@popd

@echo on

@pause
