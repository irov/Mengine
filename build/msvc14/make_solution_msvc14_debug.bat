@echo off

@pushd ..
@call make_solution "%CD%\..\CMake\Win32" solution_msvc14_debug "Visual Studio 14 2015" Debug build_msvc14_debug
@popd

@pause
@echo on
