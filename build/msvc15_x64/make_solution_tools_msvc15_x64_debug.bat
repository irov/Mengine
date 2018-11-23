@echo off

@pushd ..
@call vcvarsall_msvc15_x64.bat
@popd

@pushd ..
@call make_solution.bat "%CD%\..\CMake\Tools_Win64" solution_tools_msvc15_x64_debug "Visual Studio 15 2017 Win64" Debug build_msvc15_x64_debug
@popd

@echo on

@pause

