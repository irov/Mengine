@echo off

@call vcvarsall_msvc15

@pushd ..
@call make_solution "%CD%\..\CMake\Win32" solution_msvc15_debug "Visual Studio 15 2017" Debug build_msvc15_debug
@popd

@echo on

@pause
