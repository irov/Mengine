@echo off

@pushd ..
@call vcvarsall_msvc15.bat
@popd

@pushd ..
@call make_solution.bat "%CD%\..\CMake\Tools_Win32" solution_tools_msvc15_debug "Visual Studio 15 2017" Debug build_msvc15_debug
@popd

@echo on

@pause

